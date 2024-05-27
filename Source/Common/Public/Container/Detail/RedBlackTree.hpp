/*
Copyright 2023 creatorlxd

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
#include <initializer_list>
#include "MemoryManager.h"
#include "Utility/Utility.hpp"

/*!
@ingroup Common
@{
*/

namespace SpaceGameEngine
{
	template<typename K, typename V, typename LessComparer, typename Allocator>
	class Map;

	namespace Detail
	{
		/*!
		@brief Red-black tree implement
		@warning Do not change the node's value which will impact the node's order judgement
		*/
		template<typename V, typename LessComparer = Less<V>, typename EqualComparer = Equal<V>, typename Allocator = DefaultAllocator>
		class RedBlackTree
		{
		public:
			using ValueType = V;
			using AllocatorType = Allocator;
			using LessComparerType = LessComparer;
			using EqualComparerType = EqualComparer;

			template<typename _V, typename _LessComparer, typename _EqualComparer, typename _Allocator>
			friend class RedBlackTree;

			template<typename _K, typename _V, typename _LessComparer, typename _Allocator>
			friend class SpaceGameEngine::Map;

		private:
			struct Node
			{
				Node* m_pParent;
				Node* m_pLeftChild;
				Node* m_pRightChild;
				ValueType m_Value;
				bool m_IsRed;

				template<typename V2>
				inline Node(V2&& val)
					: m_pParent(nullptr), m_pLeftChild(nullptr), m_pRightChild(nullptr), m_Value(std::forward<V2>(val)), m_IsRed(false)
				{
				}
			};

			struct NilNodeError
			{
				inline static const ErrorMessageChar sm_pContent[] = SGE_ESTR("The node can not be nil node.");
				inline static bool Judge(const Node* pn, const Node* pnil)
				{
					return pn == pnil;
				}
			};

		public:
			inline RedBlackTree()
				: m_NilNode(*(reinterpret_cast<Node*>(&m_NilNodeContent))), m_pRoot(&m_NilNode), m_Size(0)
			{
				memset(m_NilNodeContent, 0, sizeof(m_NilNodeContent));
			}

			inline ~RedBlackTree()
			{
				SGE_ASSERT(NullPointerError, m_pRoot);
				if (m_pRoot != &m_NilNode)
					ReleaseNode(m_pRoot);
			}

			inline RedBlackTree(const RedBlackTree& t)
				: m_NilNode(*(reinterpret_cast<Node*>(&m_NilNodeContent))), m_pRoot(&m_NilNode), m_Size(t.m_Size)
			{
				memset(m_NilNodeContent, 0, sizeof(m_NilNodeContent));
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(t.m_pRoot->m_Value);
					m_pRoot->m_pParent = &m_NilNode;
					CopyNode<Allocator>(m_pRoot, t.m_pRoot, &(t.m_NilNode));
				}
			}

			inline RedBlackTree(RedBlackTree&& t)
				: m_NilNode(*(reinterpret_cast<Node*>(&m_NilNodeContent))), m_pRoot(&m_NilNode), m_Size(t.m_Size)
			{
				memset(m_NilNodeContent, 0, sizeof(m_NilNodeContent));
				if (m_Size)
				{
					m_pRoot = t.m_pRoot;
					m_pRoot->m_pParent = &m_NilNode;
					ChangeNilNodeLeaf(m_pRoot, &(t.m_NilNode));
					t.m_pRoot = &(t.m_NilNode);
					t.m_Size = 0;
				}
			}

			inline RedBlackTree& operator=(const RedBlackTree& t)
			{
				SGE_ASSERT(SelfAssignmentError, this, &t);
				RawClear();
				m_Size = t.m_Size;
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(t.m_pRoot->m_Value);
					m_pRoot->m_pParent = &m_NilNode;
					CopyNode<Allocator>(m_pRoot, t.m_pRoot, &(t.m_NilNode));
				}
				return *this;
			}

			inline RedBlackTree& operator=(RedBlackTree&& t)
			{
				SGE_ASSERT(SelfAssignmentError, this, &t);
				RawClear();
				m_Size = t.m_Size;
				if (m_Size)
				{
					m_pRoot = t.m_pRoot;
					m_pRoot->m_pParent = &m_NilNode;
					ChangeNilNodeLeaf(m_pRoot, &(t.m_NilNode));
					t.m_pRoot = &(t.m_NilNode);
					t.m_Size = 0;
				}
				return *this;
			}

			template<typename OtherAllocator>
			inline RedBlackTree(const RedBlackTree<V, LessComparer, EqualComparer, OtherAllocator>& t)
				: m_NilNode(*(reinterpret_cast<Node*>(&m_NilNodeContent))), m_pRoot(&m_NilNode), m_Size(t.m_Size)
			{
				memset(m_NilNodeContent, 0, sizeof(m_NilNodeContent));
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(t.m_pRoot->m_Value);
					m_pRoot->m_pParent = &m_NilNode;
					CopyNode<OtherAllocator>(m_pRoot, t.m_pRoot, &(t.m_NilNode));
				}
			}

			template<typename OtherAllocator>
			inline RedBlackTree(RedBlackTree<V, LessComparer, EqualComparer, OtherAllocator>&& t)
				: m_NilNode(*(reinterpret_cast<Node*>(&m_NilNodeContent))), m_pRoot(&m_NilNode), m_Size(t.m_Size)
			{
				memset(m_NilNodeContent, 0, sizeof(m_NilNodeContent));
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(std::move(t.m_pRoot->m_Value));
					m_pRoot->m_pParent = &m_NilNode;
					MoveNode<OtherAllocator>(m_pRoot, t.m_pRoot, &(t.m_NilNode));
				}
			}

			template<typename OtherAllocator>
			inline RedBlackTree& operator=(const RedBlackTree<V, LessComparer, EqualComparer, OtherAllocator>& t)
			{
				RawClear();
				m_Size = t.m_Size;
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(t.m_pRoot->m_Value);
					m_pRoot->m_pParent = &m_NilNode;
					CopyNode<OtherAllocator>(m_pRoot, t.m_pRoot, &(t.m_NilNode));
				}
				return *this;
			}

			template<typename OtherAllocator>
			inline RedBlackTree& operator=(RedBlackTree<V, LessComparer, EqualComparer, OtherAllocator>&& t)
			{
				RawClear();
				m_Size = t.m_Size;
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(std::move(t.m_pRoot->m_Value));
					m_pRoot->m_pParent = &m_NilNode;
					MoveNode<OtherAllocator>(m_pRoot, t.m_pRoot, &(t.m_NilNode));
				}
				return *this;
			}

			inline void Clear()
			{
				SGE_ASSERT(NullPointerError, m_pRoot);
				if (m_pRoot != &m_NilNode)
					ReleaseNode(m_pRoot);
				m_pRoot = &m_NilNode;
				m_Size = 0;
			}

			inline SizeType GetSize() const
			{
				return m_Size;
			}

			template<typename V2>
			inline V* Find(const V2& val)
			{
				Node* re = FindNode(val);
				if (re != &m_NilNode)
					return &(re->m_Value);
				else
					return nullptr;
			}

			template<typename V2>
			inline const V* Find(const V2& val) const
			{
				const Node* re = FindNode(val);
				if (re != &m_NilNode)
					return &(re->m_Value);
				else
					return nullptr;
			}

			/*!
			@warning Insert same value will fail so that the second part of return value is false
			@return Pair of pointer to node's value and inserted or not
			*/
			template<typename V2>
			inline Pair<V*, bool> Insert(V2&& val)
			{
				auto re = InternalInsert(std::forward<V2>(val));
				return Pair<V*, bool>(&(re.m_First->m_Value), re.m_Second);
			}

			template<typename V2>
			inline bool Remove(const V2& val)
			{
				auto pnode = FindNode(val);
				if (pnode == &m_NilNode)
					return false;
				else
				{
					RemoveNode(pnode);
					return true;
				}
			}

			template<typename Callable>
			inline void ForEach(Callable&& func)
			{
				if (m_pRoot != &m_NilNode)
				{
					Node* p = GetMinimumNode(m_pRoot);
					while (p != &m_NilNode)
					{
						func(p->m_Value);
						p = GetNextNode(p);
					}
				}
			}

			template<typename Callable>
			inline void ForEach(Callable&& func) const
			{
				if (m_pRoot != &m_NilNode)
				{
					const Node* p = GetMinimumNode(m_pRoot);
					while (p != &m_NilNode)
					{
						func(p->m_Value);
						p = GetNextNode(p);
					}
				}
			}

			template<typename Callable>
			inline void ReverseForEach(Callable&& func)
			{
				if (m_pRoot != &m_NilNode)
				{
					Node* p = GetMaximumNode(m_pRoot);
					while (p != &m_NilNode)
					{
						func(p->m_Value);
						p = GetPreviousNode(p);
					}
				}
			}

			template<typename Callable>
			inline void ReverseForEach(Callable&& func) const
			{
				if (m_pRoot != &m_NilNode)
				{
					const Node* p = GetMaximumNode(m_pRoot);
					while (p != &m_NilNode)
					{
						func(p->m_Value);
						p = GetPreviousNode(p);
					}
				}
			}

		private:
			inline void RawClear()
			{
				SGE_ASSERT(NullPointerError, m_pRoot);
				if (m_pRoot != &m_NilNode)
					ReleaseNode(m_pRoot);
			}

			inline void ReleaseNode(Node* p)
			{
				SGE_ASSERT(NullPointerError, p);
				SGE_ASSERT(NilNodeError, p, &m_NilNode);
				if (p->m_pLeftChild && p->m_pLeftChild != &m_NilNode)
					ReleaseNode(p->m_pLeftChild);
				if (p->m_pRightChild && p->m_pRightChild != &m_NilNode)
					ReleaseNode(p->m_pRightChild);
				Allocator::template Delete(p);
			}

			template<typename V2>
			inline Node* FindNode(const V2& val)
			{
				Node* p = m_pRoot;
				while (p != &m_NilNode && (!EqualComparer::Compare(p->m_Value, val)))
				{
					if (LessComparer::Compare(val, p->m_Value))
						p = p->m_pLeftChild;
					else
						p = p->m_pRightChild;
				}
				return p;
			}

			template<typename V2>
			inline const Node* FindNode(const V2& val) const
			{
				const Node* p = m_pRoot;
				while (p != &m_NilNode && (!EqualComparer::Compare(p->m_Value, val)))
				{
					if (LessComparer::Compare(val, p->m_Value))
						p = p->m_pLeftChild;
					else
						p = p->m_pRightChild;
				}
				return p;
			}

			inline void LeftRotate(Node* px)
			{
				SGE_ASSERT(NullPointerError, px);
				SGE_ASSERT(NullPointerError, px->m_pRightChild);
				SGE_ASSERT(NilNodeError, px, &m_NilNode);
				SGE_ASSERT(NilNodeError, px->m_pRightChild, &m_NilNode);
				auto py = px->m_pRightChild;
				px->m_pRightChild = py->m_pLeftChild;
				if (py->m_pLeftChild != &m_NilNode)
					py->m_pLeftChild->m_pParent = px;
				py->m_pParent = px->m_pParent;
				if (px->m_pParent == &m_NilNode)
					m_pRoot = py;
				else if (px == px->m_pParent->m_pLeftChild)
					px->m_pParent->m_pLeftChild = py;
				else
					px->m_pParent->m_pRightChild = py;
				py->m_pLeftChild = px;
				px->m_pParent = py;
			}

			inline void RightRotate(Node* px)
			{
				SGE_ASSERT(NullPointerError, px);
				SGE_ASSERT(NullPointerError, px->m_pLeftChild);
				SGE_ASSERT(NilNodeError, px, &m_NilNode);
				SGE_ASSERT(NilNodeError, px->m_pLeftChild, &m_NilNode);
				auto py = px->m_pLeftChild;
				px->m_pLeftChild = py->m_pRightChild;
				if (py->m_pRightChild != &m_NilNode)
					py->m_pRightChild->m_pParent = px;
				py->m_pParent = px->m_pParent;
				if (px->m_pParent == &m_NilNode)
					m_pRoot = py;
				else if (px == px->m_pParent->m_pLeftChild)
					px->m_pParent->m_pLeftChild = py;
				else
					px->m_pParent->m_pRightChild = py;
				py->m_pRightChild = px;
				px->m_pParent = py;
			}

			/*!
			@warning Insert same value will fail so that the second part of return value is false
			@return Pair of pointer to node and inserted or not
			*/
			template<typename V2>
			inline Pair<Node*, bool> InternalInsert(V2&& val)
			{
				Node* py = &m_NilNode;
				Node* px = m_pRoot;
				while (px != &m_NilNode && (!EqualComparer::Compare(px->m_Value, val)))
				{
					py = px;
					if (LessComparer::Compare(val, px->m_Value))
						px = px->m_pLeftChild;
					else
						px = px->m_pRightChild;
				}
				if (px == &m_NilNode)
				{
					Node* pz = Allocator::template New<Node>(std::forward<V2>(val));
					m_Size += 1;

					pz->m_pParent = py;
					if (py == &m_NilNode)
						m_pRoot = pz;
					else if (LessComparer::Compare(pz->m_Value, py->m_Value))
						py->m_pLeftChild = pz;
					else
						py->m_pRightChild = pz;
					pz->m_pLeftChild = &m_NilNode;
					pz->m_pRightChild = &m_NilNode;
					pz->m_IsRed = true;
					InsertFixUp(pz);
					return Pair<Node*, bool>(pz, true);
				}
				else
				{
					return Pair<Node*, bool>(px, false);
				}
			}

			inline void InsertFixUp(Node* pz)
			{
				SGE_ASSERT(NullPointerError, pz);
				SGE_ASSERT(NilNodeError, pz, &m_NilNode);
				while (pz->m_pParent->m_IsRed)
				{
					if (pz->m_pParent == pz->m_pParent->m_pParent->m_pLeftChild)
					{
						auto py = pz->m_pParent->m_pParent->m_pRightChild;
						if (py->m_IsRed)
						{
							pz->m_pParent->m_IsRed = false;
							py->m_IsRed = false;
							pz->m_pParent->m_pParent->m_IsRed = true;
							pz = pz->m_pParent->m_pParent;
						}
						else
						{
							if (pz == pz->m_pParent->m_pRightChild)
							{
								pz = pz->m_pParent;
								LeftRotate(pz);
							}
							pz->m_pParent->m_IsRed = false;
							pz->m_pParent->m_pParent->m_IsRed = true;
							RightRotate(pz->m_pParent->m_pParent);
						}
					}
					else
					{
						auto py = pz->m_pParent->m_pParent->m_pLeftChild;
						if (py->m_IsRed)
						{
							pz->m_pParent->m_IsRed = false;
							py->m_IsRed = false;
							pz->m_pParent->m_pParent->m_IsRed = true;
							pz = pz->m_pParent->m_pParent;
						}
						else
						{
							if (pz == pz->m_pParent->m_pLeftChild)
							{
								pz = pz->m_pParent;
								RightRotate(pz);
							}
							pz->m_pParent->m_IsRed = false;
							pz->m_pParent->m_pParent->m_IsRed = true;
							LeftRotate(pz->m_pParent->m_pParent);
						}
					}
				}
				m_pRoot->m_IsRed = false;
			}

			inline Node* GetMinimumNode(Node* p)
			{
				SGE_ASSERT(NullPointerError, p);
				while (p->m_pLeftChild != &m_NilNode)
				{
					p = p->m_pLeftChild;
				}
				return p;
			}

			inline const Node* GetMinimumNode(const Node* p) const
			{
				SGE_ASSERT(NullPointerError, p);
				while (p->m_pLeftChild != &m_NilNode)
				{
					p = p->m_pLeftChild;
				}
				return p;
			}

			inline Node* GetMaximumNode(Node* p)
			{
				SGE_ASSERT(NullPointerError, p);
				while (p->m_pRightChild != &m_NilNode)
				{
					p = p->m_pRightChild;
				}
				return p;
			}

			inline const Node* GetMaximumNode(const Node* p) const
			{
				SGE_ASSERT(NullPointerError, p);
				while (p->m_pRightChild != &m_NilNode)
				{
					p = p->m_pRightChild;
				}
				return p;
			}

			inline void Transplant(Node* pu, Node* pv)
			{
				SGE_ASSERT(NullPointerError, pu);
				SGE_ASSERT(NullPointerError, pv);
				SGE_ASSERT(NilNodeError, pu, &m_NilNode);
				// even the pv is nil node, it also can be transplanted(see the Introduction to Algorithm).

				if (pu->m_pParent == &m_NilNode)
					m_pRoot = pv;
				else if (pu == pu->m_pParent->m_pLeftChild)
					pu->m_pParent->m_pLeftChild = pv;
				else
					pu->m_pParent->m_pRightChild = pv;
				pv->m_pParent = pu->m_pParent;
			}

			inline void RemoveNode(Node* pz)
			{
				SGE_ASSERT(NullPointerError, pz);
				SGE_ASSERT(NilNodeError, pz, &m_NilNode);
				Node* py = pz;
				Node* px = nullptr;
				bool is_y_original_red = py->m_IsRed;
				if (pz->m_pLeftChild == &m_NilNode)
				{
					px = pz->m_pRightChild;
					Transplant(pz, pz->m_pRightChild);
				}
				else if (pz->m_pRightChild == &m_NilNode)
				{
					px = pz->m_pLeftChild;
					Transplant(pz, pz->m_pLeftChild);
				}
				else
				{
					py = GetMinimumNode(pz->m_pRightChild);
					is_y_original_red = py->m_IsRed;
					px = py->m_pRightChild;
					if (py->m_pParent == pz)
					{
						px->m_pParent = py;
					}
					else
					{
						Transplant(py, py->m_pRightChild);
						py->m_pRightChild = pz->m_pRightChild;
						py->m_pRightChild->m_pParent = py;
					}
					Transplant(pz, py);
					py->m_pLeftChild = pz->m_pLeftChild;
					py->m_pLeftChild->m_pParent = py;
					py->m_IsRed = pz->m_IsRed;
				}
				if (is_y_original_red == false)
					RemoveFixUp(px);
				Allocator::template Delete(pz);
				m_Size -= 1;
			}

			inline void RemoveFixUp(Node* px)
			{
				SGE_ASSERT(NullPointerError, px);
				while (px != m_pRoot && px->m_IsRed == false)
				{
					if (px == px->m_pParent->m_pLeftChild)
					{
						auto pw = px->m_pParent->m_pRightChild;
						if (pw->m_IsRed)
						{
							pw->m_IsRed = false;
							px->m_pParent->m_IsRed = true;
							LeftRotate(px->m_pParent);
							pw = px->m_pParent->m_pRightChild;
						}
						if (pw->m_pLeftChild->m_IsRed == false && pw->m_pRightChild->m_IsRed == false)
						{
							pw->m_IsRed = true;
							px = px->m_pParent;
						}
						else
						{
							if (pw->m_pRightChild->m_IsRed == false)
							{
								pw->m_pLeftChild->m_IsRed = false;
								pw->m_IsRed = true;
								RightRotate(pw);
								pw = px->m_pParent->m_pRightChild;
							}
							pw->m_IsRed = px->m_pParent->m_IsRed;
							px->m_pParent->m_IsRed = false;
							pw->m_pRightChild->m_IsRed = false;
							LeftRotate(px->m_pParent);
							px = m_pRoot;
						}
					}
					else
					{
						auto pw = px->m_pParent->m_pLeftChild;
						if (pw->m_IsRed)
						{
							pw->m_IsRed = false;
							px->m_pParent->m_IsRed = true;
							RightRotate(px->m_pParent);
							pw = px->m_pParent->m_pLeftChild;
						}
						if (pw->m_pRightChild->m_IsRed == false && pw->m_pLeftChild->m_IsRed == false)
						{
							pw->m_IsRed = true;
							px = px->m_pParent;
						}
						else
						{
							if (pw->m_pLeftChild->m_IsRed == false)
							{
								pw->m_pRightChild->m_IsRed = false;
								pw->m_IsRed = true;
								LeftRotate(pw);
								pw = px->m_pParent->m_pLeftChild;
							}
							pw->m_IsRed = px->m_pParent->m_IsRed;
							px->m_pParent->m_IsRed = false;
							pw->m_pLeftChild->m_IsRed = false;
							RightRotate(px->m_pParent);
							px = m_pRoot;
						}
					}
				}
				px->m_IsRed = false;
			}

			inline void ChangeNilNodeLeaf(Node* p, Node* pother_nil)
			{
				SGE_ASSERT(NullPointerError, p);
				SGE_ASSERT(NullPointerError, pother_nil);
				if (p->m_pLeftChild == pother_nil)
					p->m_pLeftChild = &m_NilNode;
				else
					ChangeNilNodeLeaf(p->m_pLeftChild, pother_nil);

				if (p->m_pRightChild == pother_nil)
					p->m_pRightChild = &m_NilNode;
				else
					ChangeNilNodeLeaf(p->m_pRightChild, pother_nil);
			}

			template<typename OtherAllocator>
			inline void CopyNode(Node* pnow, const typename RedBlackTree<V, LessComparer, EqualComparer, OtherAllocator>::Node* pother, const typename RedBlackTree<V, LessComparer, EqualComparer, OtherAllocator>::Node* pother_nil)
			{
				SGE_ASSERT(NullPointerError, pnow);
				SGE_ASSERT(NullPointerError, pother);
				SGE_ASSERT(NullPointerError, pother_nil);
				SGE_ASSERT(NilNodeError, pnow, &m_NilNode);
				using AnotherNilNodeError = typename SpaceGameEngine::Detail::RedBlackTree<V, LessComparer, EqualComparer, OtherAllocator>::NilNodeError;
				SGE_ASSERT(AnotherNilNodeError, pother, pother_nil);

				if (pother->m_pLeftChild != pother_nil)
				{
					Node* pleft = Allocator::template New<Node>(pother->m_pLeftChild->m_Value);
					pleft->m_pParent = pnow;
					pnow->m_pLeftChild = pleft;
					CopyNode<OtherAllocator>(pleft, pother->m_pLeftChild, pother_nil);
				}
				else
					pnow->m_pLeftChild = &m_NilNode;

				if (pother->m_pRightChild != pother_nil)
				{
					Node* pright = Allocator::template New<Node>(pother->m_pRightChild->m_Value);
					pright->m_pParent = pnow;
					pnow->m_pRightChild = pright;
					CopyNode<OtherAllocator>(pright, pother->m_pRightChild, pother_nil);
				}
				else
					pnow->m_pRightChild = &m_NilNode;
			}

			template<typename OtherAllocator>
			inline void MoveNode(Node* pnow, const typename RedBlackTree<V, LessComparer, EqualComparer, OtherAllocator>::Node* pother, const typename RedBlackTree<V, LessComparer, EqualComparer, OtherAllocator>::Node* pother_nil)
			{
				SGE_ASSERT(NullPointerError, pnow);
				SGE_ASSERT(NullPointerError, pother);
				SGE_ASSERT(NullPointerError, pother_nil);
				SGE_ASSERT(NilNodeError, pnow, &m_NilNode);
				using AnotherNilNodeError = typename SpaceGameEngine::Detail::RedBlackTree<V, LessComparer, EqualComparer, OtherAllocator>::NilNodeError;
				SGE_ASSERT(AnotherNilNodeError, pother, pother_nil);

				if (pother->m_pLeftChild != pother_nil)
				{
					Node* pleft = Allocator::template New<Node>(std::move(pother->m_pLeftChild->m_Value));
					pleft->m_pParent = pnow;
					pnow->m_pLeftChild = pleft;
					MoveNode<OtherAllocator>(pleft, pother->m_pLeftChild, pother_nil);
				}
				else
					pnow->m_pLeftChild = &m_NilNode;

				if (pother->m_pRightChild != pother_nil)
				{
					Node* pright = Allocator::template New<Node>(std::move(pother->m_pRightChild->m_Value));
					pright->m_pParent = pnow;
					pnow->m_pRightChild = pright;
					MoveNode<OtherAllocator>(pright, pother->m_pRightChild, pother_nil);
				}
				else
					pnow->m_pRightChild = &m_NilNode;
			}

			inline Node* GetNextNode(Node* p)
			{
				SGE_ASSERT(NullPointerError, p);
				SGE_ASSERT(NilNodeError, p, &m_NilNode);
				if (p->m_pRightChild != &m_NilNode)
				{
					return GetMinimumNode(p->m_pRightChild);
				}
				else
				{
					while (p->m_pParent != &m_NilNode)
					{
						if (p == p->m_pParent->m_pLeftChild)
							return p->m_pParent;
						else
							p = p->m_pParent;
					}
					return &m_NilNode;
				}
			}

			inline const Node* GetNextNode(const Node* p) const
			{
				SGE_ASSERT(NullPointerError, p);
				SGE_ASSERT(NilNodeError, p, &m_NilNode);
				if (p->m_pRightChild != &m_NilNode)
				{
					return GetMinimumNode(p->m_pRightChild);
				}
				else
				{
					while (p->m_pParent != &m_NilNode)
					{
						if (p == p->m_pParent->m_pLeftChild)
							return p->m_pParent;
						else
							p = p->m_pParent;
					}
					return &m_NilNode;
				}
			}

			inline Node* GetPreviousNode(Node* p)
			{
				SGE_ASSERT(NullPointerError, p);
				SGE_ASSERT(NilNodeError, p, &m_NilNode);
				if (p->m_pLeftChild != &m_NilNode)
				{
					return GetMaximumNode(p->m_pLeftChild);
				}
				else
				{
					while (p->m_pParent != &m_NilNode)
					{
						if (p == p->m_pParent->m_pRightChild)
							return p->m_pParent;
						else
							p = p->m_pParent;
					}
					return &m_NilNode;
				}
			}

			inline const Node* GetPreviousNode(const Node* p) const
			{
				SGE_ASSERT(NullPointerError, p);
				SGE_ASSERT(NilNodeError, p, &m_NilNode);
				if (p->m_pLeftChild != &m_NilNode)
				{
					return GetMaximumNode(p->m_pLeftChild);
				}
				else
				{
					while (p->m_pParent != &m_NilNode)
					{
						if (p == p->m_pParent->m_pRightChild)
							return p->m_pParent;
						else
							p = p->m_pParent;
					}
					return &m_NilNode;
				}
			}

		private:
			Node& m_NilNode;
			alignas(Node) UInt8 m_NilNodeContent[sizeof(Node)];
			Node* m_pRoot;
			SizeType m_Size;
		};
	}
}

/*!
@}
*/