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
		inline static constexpr const bool IsVisitorForIndex = requires(T t) {
			t.template Visit<Index>(std::declval<typename Types::template Get<Index>>());
		};

		template<typename T, typename IndexSequence>
		struct IsVisitorForIndices
		{
			inline static constexpr const bool Value = false;
		};

		template<typename T, SizeType... Indices>
		struct IsVisitorForIndices<T, std::integer_sequence<SizeType, Indices...>>
		{
			inline static constexpr const bool Value = (... && IsVisitorForIndex<T, Indices>);
		};

		template<typename T, SizeType Index>
		using VisitorResultTypeForIndex = decltype(std::declval<T>().template Visit<Index>(std::declval<typename Types::template Get<Index>>()));

		template<typename T, typename IndexSequence>
		struct IsVisitorResultForIndicesSame
		{
			inline static constexpr const bool Value = false;
		};

		template<typename T, SizeType... Indices>
		struct IsVisitorResultForIndicesSame<T, std::integer_sequence<SizeType, Indices...>>
		{
			inline static constexpr const bool Value = (... && std::is_same_v<VisitorResultTypeForIndex<T, Indices>, VisitorResultTypeForIndex<T, 0>>);
		};

	public:
		template<typename T>
		inline static constexpr const bool IsVisitor = IsVisitorForIndices<T, std::make_integer_sequence<SizeType, Types::Size>>::Value && IsVisitorResultForIndicesSame<T, std::make_integer_sequence<SizeType, Types::Size>>::Value;

	private:
		template<typename VisitorType>
			requires IsVisitor<VisitorType>
		using VisitResultType = VisitorResultTypeForIndex<VisitorType, 0>;	  // all result type must be same

		template<typename VisitorType>
			requires IsVisitor<VisitorType>
		using VisitorFunctionType = VisitResultType<VisitorType>(*)(VisitorType&&, Byte*);

		template<typename VisitorType>
			requires IsVisitor<VisitorType>
		using ConstVisitorFunctionType = VisitResultType<VisitorType>(*)(VisitorType&&, const Byte*);

		template<typename VisitorType, SizeType Index>
			requires IsVisitor<VisitorType>
		struct GetVisitorFunctionMetaFunction
		{
			inline static constexpr const VisitorFunctionType<VisitorType> Value = [](VisitorType&& vistor, Byte* ptr) -> VisitResultType<VisitorType> {
				return std::forward<VisitorType>(vistor).template Visit<Index>(*reinterpret_cast<typename Types::template Get<Index>*>(ptr));
			};
		};

		template<typename VisitorType, SizeType Index>
			requires IsVisitor<VisitorType>
		struct GetConstVisitorFunctionMetaFunction
		{
			inline static constexpr const ConstVisitorFunctionType<VisitorType> Value = [](VisitorType&& vistor, const Byte* ptr) -> VisitResultType<VisitorType> {
				return std::forward<VisitorType>(vistor).template Visit<Index>(*reinterpret_cast<const typename Types::template Get<Index>*>(ptr));
			};
		};

		template<typename VisitorType, typename IndexSequence>
			requires IsVisitor<VisitorType>
		struct GetVisitorFunctionArrayMetaFunction
		{
		};

		template<typename VisitorType, SizeType... Indices>
			requires IsVisitor<VisitorType>
		struct GetVisitorFunctionArrayMetaFunction<VisitorType, std::integer_sequence<SizeType, Indices...>>
		{
			inline static constexpr const VisitorFunctionType<VisitorType> Value[] = {GetVisitorFunctionMetaFunction<VisitorType, Indices>::Value...};
		};

		template<typename VisitorType, typename IndexSequence>
			requires IsVisitor<VisitorType>
		struct GetConstVisitorFunctionArrayMetaFunction
		{
		};

		template<typename VisitorType, SizeType... Indices>
			requires IsVisitor<VisitorType>
		struct GetConstVisitorFunctionArrayMetaFunction<VisitorType, std::integer_sequence<SizeType, Indices...>>
		{
			inline static constexpr const ConstVisitorFunctionType<VisitorType> Value[] = {GetConstVisitorFunctionMetaFunction<VisitorType, Indices>::Value...};
		};

	public:
		inline Variant()
		{
			static_assert(FirstDefaultConstructibleIndex < Types::Size, "No default constructible type in Variant.");
			using DefaultType = typename Types::template Get<FirstDefaultConstructibleIndex>;
			new (m_Content) DefaultType();
		}

		template<SizeType Index, typename... Args>
		inline Variant(ValueWrapper<Index>, Args&&... args)
			: m_TypeIndex(Index)
		{
			static_assert(Index < Types::Size, "Index out of bounds in Variant constructor.");
			using Type = typename Types::template Get<Index>;
			new (m_Content) Type(std::forward<Args>(args)...);
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

		template<typename VisitorType>
			requires IsVisitor<VisitorType>
		inline decltype(auto) Visit(VisitorType&& vistor)
		{
			static auto vistor_functions = GetVisitorFunctionArrayMetaFunction<VisitorType, std::make_integer_sequence<SizeType, Types::Size>>::Value;
			return vistor_functions[m_TypeIndex](std::forward<VisitorType>(vistor), m_Content);
		}

		template<typename VisitorType>
			requires IsVisitor<VisitorType>
		inline decltype(auto) Visit(VisitorType&& vistor) const
		{
			static auto vistor_functions = GetConstVisitorFunctionArrayMetaFunction<VisitorType, std::make_integer_sequence<SizeType, Types::Size>>::Value;
			return vistor_functions[m_TypeIndex](std::forward<VisitorType>(vistor), m_Content);
		}

	private:
		alignas(Alignment) Byte m_Content[Size];
		SizeType m_TypeIndex = FirstDefaultConstructibleIndex;
	};

	template<typename VariantType, typename VisitorType>
	concept IsVariantVisitor = VariantType::template IsVisitor<VisitorType>;

	template<typename... Ts>
		requires((... && !std::is_reference_v<Ts>) && IsTypesUnique<Ts...>::Value)
	class UniqueVariant : public Variant<Ts...>
	{
	private:
		template<typename VisitorType, typename T>
		inline static constexpr const bool IsVisitorForType = requires(VisitorType visitor) {
			visitor(std::declval<T>());
		};

		template<typename VisitorType, typename T>
		using VisitorResultTypeForType = decltype(std::declval<VisitorType>()(std::declval<T>()));

	public:
		template<typename VisitorType>
		inline static constexpr const bool IsVisitor = (... && IsVisitorForType<VisitorType, Ts>)&&(... && std::is_same_v<VisitorResultTypeForType<VisitorType, Ts>, VisitorResultTypeForType<VisitorType, typename Variant<Ts...>::Types::template Get<0>>>);

	private:
		template<typename VisitorType>
			requires IsVisitor<VisitorType>
		class VisitorAdaptor
		{
		public:
			inline VisitorAdaptor(VisitorType&& visitor)
				: m_Visitor(std::forward<VisitorType>(visitor))
			{
			}

			template<SizeType Index, typename T>
			inline decltype(auto) Visit(T&& value) const
			{
				return std::forward<VisitorType>(m_Visitor)(std::forward<T>(value));
			}

		private:
			VisitorType&& m_Visitor;
		};

		template<typename T>
		struct IsSameWith
		{
			template<typename U>
			struct Type
			{
				inline static constexpr const bool Value = std::is_same_v<T, U>;
			};
		};

	public:
		inline UniqueVariant() = default;
		inline UniqueVariant(const UniqueVariant&) = default;
		inline UniqueVariant(UniqueVariant&&) = default;
		inline UniqueVariant& operator=(const UniqueVariant&) = default;
		inline UniqueVariant& operator=(UniqueVariant&&) = default;

		template<typename T, typename... Args>
		inline UniqueVariant(TypeWrapper<T>, Args&&... args)
			: Variant<Ts...>(InPlaceIndex<Variant<Ts...>::Types::template FirstIndex<IsSameWith<T>::template Type>>, std::forward<Args>(args)...)
		{
			static constexpr const SizeType Index = Variant<Ts...>::Types::template FirstIndex<IsSameWith<T>::template Type>;
			static_assert(Index < Variant<Ts...>::Types::Size, "Type T is not in UniqueVariant.");
		}

		template<typename T, typename... Args>
		inline std::remove_reference_t<T>& Emplace(Args&&... args)
		{
			static constexpr const SizeType Index = Variant<Ts...>::Types::template FirstIndex<IsSameWith<T>::template Type>;
			static_assert(Index < Variant<Ts...>::Types::Size, "Type T is not in UniqueVariant.");
			return Variant<Ts...>::template Emplace<Index>(std::forward<Args>(args)...);
		}

		template<typename T>
		inline std::remove_reference_t<T>& Get()
		{
			static constexpr const SizeType Index = Variant<Ts...>::Types::template FirstIndex<IsSameWith<T>::template Type>;
			static_assert(Index < Variant<Ts...>::Types::Size, "Type T is not in UniqueVariant.");
			return Variant<Ts...>::template Get<Index>();
		}

		template<typename T>
		inline const std::remove_reference_t<T>& Get() const
		{
			static constexpr const SizeType Index = Variant<Ts...>::Types::template FirstIndex<IsSameWith<T>::template Type>;
			static_assert(Index < Variant<Ts...>::Types::Size, "Type T is not in UniqueVariant.");
			return Variant<Ts...>::template Get<Index>();
		}

		template<typename T>
		inline std::remove_reference_t<T>* Query()
		{
			static constexpr const SizeType Index = Variant<Ts...>::Types::template FirstIndex<IsSameWith<T>::template Type>;
			static_assert(Index < Variant<Ts...>::Types::Size, "Type T is not in UniqueVariant.");
			return Variant<Ts...>::template Query<Index>();
		}

		template<typename T>
		inline const std::remove_reference_t<T>* Query() const
		{
			static constexpr const SizeType Index = Variant<Ts...>::Types::template FirstIndex<IsSameWith<T>::template Type>;
			static_assert(Index < Variant<Ts...>::Types::Size, "Type T is not in UniqueVariant.");
			return Variant<Ts...>::template Query<Index>();
		}

		template<typename VisitorType>
			requires IsVisitor<VisitorType>
		inline decltype(auto) Visit(VisitorType&& visitor)
		{
			VisitorAdaptor<VisitorType> adaptor(std::forward<VisitorType>(visitor));
			return Variant<Ts...>::Visit(ForwardLike<VisitorType>(adaptor));
		}

		template<typename VisitorType>
			requires IsVisitor<VisitorType>
		inline decltype(auto) Visit(VisitorType&& visitor) const
		{
			VisitorAdaptor<VisitorType> adaptor(std::forward<VisitorType>(visitor));
			return Variant<Ts...>::Visit(ForwardLike<VisitorType>(adaptor));
		}
	};

	template<typename UniqueVariantType, typename VisitorType>
	concept IsUniqueVariantVisitor = UniqueVariantType::template IsVisitor<VisitorType>;
}

/*!
@}
*/