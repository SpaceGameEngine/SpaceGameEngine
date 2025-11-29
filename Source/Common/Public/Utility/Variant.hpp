/*
Copyright 2025 creatorlxd

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma once
#include "Utility/Utility.hpp"
#include "Meta/TypeList.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	namespace Detail
	{
		template<typename T, typename... Ts>
		struct MaxSizeOfTypes
		{
			inline static constexpr const SizeType Value = Max<SizeType>(sizeof(T), MaxSizeOfTypes<Ts...>::Value);
		};

		template<typename T>
		struct MaxSizeOfTypes<T>
		{
			inline static constexpr const SizeType Value = sizeof(T);
		};

		template<typename T, typename... Ts>
		struct MaxAlignmentOfTypes
		{
			inline static constexpr const SizeType Value = Max<SizeType>(alignof(T), MaxAlignmentOfTypes<Ts...>::Value);
		};

		template<typename T>
		struct MaxAlignmentOfTypes<T>
		{
			inline static constexpr const SizeType Value = alignof(T);
		};
	}

	template<typename... Ts>
		requires(... && !std::is_reference_v<Ts>)
	class Variant
	{
	public:
		using Types = TypeList<Ts...>;
		inline static constexpr const SizeType Size = Detail::MaxSizeOfTypes<Ts...>::Value;
		inline static constexpr const SizeType Alignment = Detail::MaxAlignmentOfTypes<Ts...>::Value;

	private:
		inline static const ErrorMessageChar InvalidOperationErrorMessage[] = SGE_ESTR("This type does not support this operation.");

		template<typename T>
		struct IsDefaultConstructibleMetaFunction
		{
			inline static constexpr const bool Value = std::is_default_constructible_v<T>;
		};
		inline static constexpr const SizeType FirstDefaultConstructibleIndex = Types::template FirstIndex<IsDefaultConstructibleMetaFunction>;

		using DestructorFunctionType = void (*)(Byte*);
		template<typename T>
		struct GetDestructorMetaFunction
		{
			inline static constexpr const DestructorFunctionType Value = [](Byte* ptr) {
				reinterpret_cast<T*>(ptr)->~T();
			};
		};
		inline static constexpr const DestructorFunctionType Destructors[] = {GetDestructorMetaFunction<Ts>::Value...};

		using CopyConstructorFunctionType = void (*)(Byte*, const Byte*);
		template<typename T>
		struct GetCopyConstructorMetaFunction
		{
			inline static constexpr const CopyConstructorFunctionType Value = [](Byte* pdst, const Byte* psrc) {
				if constexpr (std::is_copy_constructible_v<T>)
					new (pdst) T(*reinterpret_cast<const T*>(psrc));
				else
					SGE_THROW_ERROR(InvalidOperationErrorMessage);
			};
		};
		inline static constexpr const CopyConstructorFunctionType CopyConstructors[] = {GetCopyConstructorMetaFunction<Ts>::Value...};

		using MoveConstructorFunctionType = void (*)(Byte*, Byte*);
		template<typename T>
		struct GetMoveConstructorMetaFunction
		{
			inline static constexpr const MoveConstructorFunctionType Value = [](Byte* pdst, Byte* psrc) {
				if constexpr (std::is_move_constructible_v<T>)
					new (pdst) T(std::move(*reinterpret_cast<T*>(psrc)));
				else
					SGE_THROW_ERROR(InvalidOperationErrorMessage);
			};
		};
		inline static constexpr const MoveConstructorFunctionType MoveConstructors[] = {GetMoveConstructorMetaFunction<Ts>::Value...};

		using CopyAssignmentFunctionType = void (*)(Byte*, const Byte*);
		template<typename T>
		struct GetCopyAssignmentMetaFunction
		{
			inline static constexpr const CopyAssignmentFunctionType Value = [](Byte* pdst, const Byte* psrc) {
				if constexpr (std::is_copy_assignable_v<T>)
					*reinterpret_cast<T*>(pdst) = *reinterpret_cast<const T*>(psrc);
				else
					SGE_THROW_ERROR(InvalidOperationErrorMessage);
			};
		};
		inline static constexpr const CopyAssignmentFunctionType CopyAssignments[] = {GetCopyAssignmentMetaFunction<Ts>::Value...};

		using MoveAssignmentFunctionType = void (*)(Byte*, Byte*);
		template<typename T>
		struct GetMoveAssignmentMetaFunction
		{
			inline static constexpr const MoveAssignmentFunctionType Value = [](Byte* pdst, Byte* psrc) {
				if constexpr (std::is_move_assignable_v<T>)
					*reinterpret_cast<T*>(pdst) = std::move(*reinterpret_cast<T*>(psrc));
				else
					SGE_THROW_ERROR(InvalidOperationErrorMessage);
			};
		};
		inline static constexpr const MoveAssignmentFunctionType MoveAssignments[] = {GetMoveAssignmentMetaFunction<Ts>::Value...};

		struct IncorrectTypeIndexError
		{
			inline static const ErrorMessageChar pContent[] = SGE_ESTR("The type index is incorrect.");
			inline static bool Judge(const Variant& variant, SizeType idx)
			{
				return variant.GetTypeIndex() != idx;
			}
		};

		template<typename T, SizeType Index>
		inline static constexpr const bool IsVistorForIndex = requires(T t) {
			t.Visit<Index>(std::declval<typename Types::template Get<Index>>());
		};

		template<typename T, typename IndexSequence>
		struct IsVistorForIndices
		{
			inline static constexpr const bool Value = false;
		};

		template<typename T, SizeType... Indices>
		struct IsVistorForIndices<T, std::integer_sequence<SizeType, Indices...>>
		{
			inline static constexpr const bool Value = (... && IsVistorForIndex<T, Indices>);
		};

		template<typename T, SizeType Index>
		using VistorResultTypeForIndex = decltype(std::declval<T>().template Visit<Index>(std::declval<typename Types::template Get<Index>>()));

		template<typename T, typename IndexSequence>
		struct IsVistorResultForIndicesSame
		{
			inline static constexpr const bool Value = false;
		};

		template<typename T, SizeType... Indices>
		struct IsVistorResultForIndicesSame<T, std::integer_sequence<SizeType, Indices...>>
		{
			inline static constexpr const bool Value = (... && std::is_same_v<VistorResultTypeForIndex<T, Indices>, VistorResultTypeForIndex<T, 0>>);
		};

		template<typename VistorType>
			requires IsVistor<VistorType>
		using VisitResultType = VistorResultTypeForIndex<VistorType, 0>;	// all result type must be same

		template<typename VistorType>
			requires IsVistor<VistorType>
		using VistorFunctionType = VisitResultType<VistorType>(*)(VistorType&&, Byte*);

		template<typename VistorType>
			requires IsVistor<VistorType>
		using ConstVistorFunctionType = VisitResultType<VistorType>(*)(VistorType&&, const Byte*);

		template<typename VistorType, SizeType Index>
			requires IsVistor<VistorType>
		struct GetVistorFunctionMetaFunction
		{
			inline static constexpr const VistorFunctionType<VistorType> Value = [](VistorType&& vistor, Byte* ptr) -> VisitResultType<VistorType> {
				return std::forward<VistorType>(vistor).template Visit<Index>(*reinterpret_cast<typename Types::template Get<Index>*>(ptr));
			};
		};

		template<typename VistorType, SizeType Index>
			requires IsVistor<VistorType>
		struct GetConstVistorFunctionMetaFunction
		{
			inline static constexpr const ConstVistorFunctionType<VistorType> Value = [](VistorType&& vistor, const Byte* ptr) -> VisitResultType<VistorType> {
				return std::forward<VistorType>(vistor).template Visit<Index>(*reinterpret_cast<const typename Types::template Get<Index>*>(ptr));
			};
		};

		template<typename VistorType, typename IndexSequence>
			requires IsVistor<VistorType>
		struct GetVistorFunctionArrayMetaFunction
		{
		};

		template<typename VistorType, SizeType... Indices>
			requires IsVistor<VistorType>
		struct GetVistorFunctionArrayMetaFunction<VistorType, std::integer_sequence<SizeType, Indices...>>
		{
			inline static constexpr const VistorFunctionType<VistorType> Value[] = {GetVistorFunctionMetaFunction<VistorType, Indices>::Value...};
		};

		template<typename VistorType, typename IndexSequence>
			requires IsVistor<VistorType>
		struct GetConstVistorFunctionArrayMetaFunction
		{
		};

		template<typename VistorType, SizeType... Indices>
			requires IsVistor<VistorType>
		struct GetConstVistorFunctionArrayMetaFunction<VistorType, std::integer_sequence<SizeType, Indices...>>
		{
			inline static constexpr const ConstVistorFunctionType<VistorType> Value[] = {GetConstVistorFunctionMetaFunction<VistorType, Indices>::Value...};
		};

	public:
		template<typename T>
		inline static constexpr const bool IsVistor = IsVistorForIndices<T, std::make_integer_sequence<SizeType, Types::Size>>::Value && IsVistorResultForIndicesSame<T, std::make_integer_sequence<SizeType, Types::Size>>::Value;

	public:
		inline Variant()
		{
			static_assert(FirstDefaultConstructibleIndex < Types::Size, "No default constructible type in Variant.");
			using DefaultType = typename Types::template Get<FirstDefaultConstructibleIndex>;
			new (m_Content) DefaultType();
		}

		inline ~Variant()
		{
			Destructors[m_TypeIndex](m_Content);
		}

		inline Variant(const Variant& variant)
			: m_TypeIndex(variant.m_TypeIndex)
		{
			CopyConstructors[m_TypeIndex](m_Content, variant.m_Content);
		}

		inline Variant(Variant&& variant)
			: m_TypeIndex(variant.m_TypeIndex)
		{
			MoveConstructors[m_TypeIndex](m_Content, variant.m_Content);
		}

		inline Variant& operator=(const Variant& variant)
		{
			SGE_ASSERT(SelfAssignmentError, this, &variant);

			if (m_TypeIndex == variant.m_TypeIndex)
				CopyAssignments[m_TypeIndex](m_Content, variant.m_Content);
			else
			{
				Destructors[m_TypeIndex](m_Content);
				m_TypeIndex = variant.m_TypeIndex;
				CopyConstructors[m_TypeIndex](m_Content, variant.m_Content);
			}
			return *this;
		}

		inline Variant& operator=(Variant&& variant)
		{
			SGE_ASSERT(SelfAssignmentError, this, &variant);

			if (m_TypeIndex == variant.m_TypeIndex)
				MoveAssignments[m_TypeIndex](m_Content, variant.m_Content);
			else
			{
				Destructors[m_TypeIndex](m_Content);
				m_TypeIndex = variant.m_TypeIndex;
				MoveConstructors[m_TypeIndex](m_Content, variant.m_Content);
			}
			return *this;
		}

		inline SizeType GetTypeIndex() const
		{
			return m_TypeIndex;
		}

		template<SizeType Index, typename... Args>
		inline typename Types::template Get<Index>& Emplace(Args&&... args)
		{
			static_assert(Index < Types::Size, "Index out of bounds in Variant::Emplace.");
			Destructors[m_TypeIndex](m_Content);
			m_TypeIndex = Index;
			using Type = typename Types::template Get<Index>;
			new (m_Content) Type(std::forward<Args>(args)...);
			return *reinterpret_cast<Type*>(m_Content);
		}

		template<SizeType Index>
		inline typename Types::template Get<Index>& Get()
		{
			static_assert(Index < Types::Size, "Index out of bounds in Variant::Get.");
			SGE_CHECK(IncorrectTypeIndexError, *this, Index);
			return *reinterpret_cast<typename Types::template Get<Index>*>(m_Content);
		}

		template<SizeType Index>
		inline const typename Types::template Get<Index>& Get() const
		{
			static_assert(Index < Types::Size, "Index out of bounds in Variant::Get.");
			SGE_CHECK(IncorrectTypeIndexError, *this, Index);
			return *reinterpret_cast<const typename Types::template Get<Index>*>(m_Content);
		}

		template<SizeType Index>
		inline typename Types::template Get<Index>* Query()
		{
			static_assert(Index < Types::Size, "Index out of bounds in Variant::Query.");
			if (m_TypeIndex != Index)
				return nullptr;
			return reinterpret_cast<typename Types::template Get<Index>*>(m_Content);
		}

		template<SizeType Index>
		inline const typename Types::template Get<Index>* Query() const
		{
			static_assert(Index < Types::Size, "Index out of bounds in Variant::Query.");
			if (m_TypeIndex != Index)
				return nullptr;
			return reinterpret_cast<const typename Types::template Get<Index>*>(m_Content);
		}

		template<typename VistorType>
			requires IsVistor<VistorType>
		inline decltype(auto) Visit(VistorType&& vistor)
		{
			static auto vistor_functions = GetVistorFunctionArrayMetaFunction<VistorType, std::make_integer_sequence<SizeType, Types::Size>>::Value;
			return vistor_functions[m_TypeIndex](std::forward<VistorType>(vistor), m_Content);
		}

		template<typename VistorType>
			requires IsVistor<VistorType>
		inline decltype(auto) Visit(VistorType&& vistor) const
		{
			static auto vistor_functions = GetConstVistorFunctionArrayMetaFunction<VistorType, std::make_integer_sequence<SizeType, Types::Size>>::Value;
			return vistor_functions[m_TypeIndex](std::forward<VistorType>(vistor), m_Content);
		}

	private:
		alignas(Alignment) Byte m_Content[Size];
		SizeType m_TypeIndex = FirstDefaultConstructibleIndex;
	};

	template<typename VariantType, typename VistorType>
	concept IsVariantVistor = VariantType::template IsVistor<VistorType>;
}

/*!
@}
*/