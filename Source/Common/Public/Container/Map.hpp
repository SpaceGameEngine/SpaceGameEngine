/*
Copyright 2021 creatorlxd

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

namespace SpaceGameEngine
{
	/*!
	@ingroup Common
	@{
	*/

	template<typename K, typename V, typename LessComparer, typename Allocator>
	class Map;

	namespace MapImplement
	{
		template<typename K, typename V, typename LessComparer = Less<K>, typename Allocator = DefaultAllocator>
		class RedBlackTree
		{
		public:
			using KeyType = const K;
			using ValueType = V;
			using AllocatorType = Allocator;
			using LessComparerType = LessComparer;

			template<typename _K, typename _V, typename _LessComparer, typename _Allocator>
			friend class RedBlackTree;

			friend class Map<K, V, LessComparer, Allocator>;

		private:
			struct Node
			{
				Node* m_pParent;
				Node* m_pLeftChild;
				Node* m_pRightChild;
				Pair<const K, V> m_KeyValuePair;
				bool m_IsRed;

				inline Node()
					: m_pParent(nullptr), m_pLeftChild(nullptr), m_pRightChild(nullptr), m_KeyValuePair(K(), V()), m_IsRed(false)
				{
				}

				template<typename K2, typename V2>
				inline Node(K2&& key, V2&& val)
					: m_pParent(nullptr), m_pLeftChild(nullptr), m_pRightChild(nullptr), m_KeyValuePair(std::forward<K2>(key), std::forward<V2>(val)), m_IsRed(false)
				{
				}

				template<typename P>
				inline explicit Node(P&& p)
					: m_pParent(nullptr), m_pLeftChild(nullptr), m_pRightChild(nullptr), m_KeyValuePair(std::forward<P>(p)), m_IsRed(false)
				{
				}
			};

			struct NilNodeError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("The node can not be nil node.");
				inline static bool Judge(const Node* pn, const Node* pnil)
				{
					return pn == pnil;
				}
			};

		public:
			inline RedBlackTree()
				: m_pRoot(&m_NilNode), m_Size(0)
			{
			}

			inline ~RedBlackTree()
			{
				SGE_ASSERT(NullPointerError, m_pRoot);
				if (m_pRoot != &m_NilNode)
					ReleaseNode(m_pRoot);
			}

			inline RedBlackTree(const RedBlackTree& t)
				: m_pRoot(&m_NilNode), m_Size(t.m_Size)
			{
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(t.m_pRoot->m_KeyValuePair);
					m_pRoot->m_pParent = &m_NilNode;
					CopyNode<Allocator>(m_pRoot, t.m_pRoot, &(t.m_NilNode));
				}
			}

			inline RedBlackTree(RedBlackTree&& t)
				: m_pRoot(&m_NilNode), m_Size(t.m_Size)
			{
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
				Clear();
				m_Size = t.m_Size;
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(t.m_pRoot->m_KeyValuePair);
					m_pRoot->m_pParent = &m_NilNode;
					CopyNode<Allocator>(m_pRoot, t.m_pRoot, &(t.m_NilNode));
				}
				return *this;
			}

			inline RedBlackTree& operator=(RedBlackTree&& t)
			{
				SGE_ASSERT(SelfAssignmentError, this, &t);
				Clear();
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
			inline RedBlackTree(const RedBlackTree<K, V, LessComparer, OtherAllocator>& t)
				: m_pRoot(&m_NilNode), m_Size(t.m_Size)
			{
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(t.m_pRoot->m_KeyValuePair);
					m_pRoot->m_pParent = &m_NilNode;
					CopyNode<OtherAllocator>(m_pRoot, t.m_pRoot, &(t.m_NilNode));
				}
			}

			template<typename OtherAllocator>
			inline RedBlackTree(RedBlackTree<K, V, LessComparer, OtherAllocator>&& t)
				: m_pRoot(&m_NilNode), m_Size(t.m_Size)
			{
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(std::move(t.m_pRoot->m_KeyValuePair));
					m_pRoot->m_pParent = &m_NilNode;
					MoveNode<OtherAllocator>(m_pRoot, t.m_pRoot, &(t.m_NilNode));
				}
			}

			template<typename OtherAllocator>
			inline RedBlackTree& operator=(const RedBlackTree<K, V, LessComparer, OtherAllocator>& t)
			{
				Clear();
				m_Size = t.m_Size;
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(t.m_pRoot->m_KeyValuePair);
					m_pRoot->m_pParent = &m_NilNode;
					CopyNode<OtherAllocator>(m_pRoot, t.m_pRoot, &(t.m_NilNode));
				}
				return *this;
			}

			template<typename OtherAllocator>
			inline RedBlackTree& operator=(RedBlackTree<K, V, LessComparer, OtherAllocator>&& t)
			{
				Clear();
				m_Size = t.m_Size;
				if (m_Size)
				{
					m_pRoot = Allocator::template New<Node>(std::move(t.m_pRoot->m_KeyValuePair));
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

			inline V* FindValue(const K& key)
			{
				Node* re = FindNode(key);
				if (re != &m_NilNode)
					return &(re->m_KeyValuePair.m_Second);
				else
					return nullptr;
			}

			inline const V* FindValue(const K& key) const
			{
				const Node* re = FindNode(key);
				if (re != &m_NilNode)
					return &(re->m_KeyValuePair.m_Second);
				else
					return nullptr;
			}

			template<typename K2, typename V2>
			inline Pair<Pair<const K, V>*, bool> Insert(K2&& key, V2&& val)
			{
				auto re = InternalInsert(std::forward<K2>(key), std::forward<V2>(val));
				return Pair<Pair<const K, V>*, bool>(&(re.m_First->m_KeyValuePair), re.m_Second);
			}

			inline bool RemoveByKey(const K& key)
			{
				auto pnode = FindNode(key);
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
						func(p->m_KeyValuePair);
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
						func(p->m_KeyValuePair);
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
						func(p->m_KeyValuePair);
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
						func(p->m_KeyValuePair);
						p = GetPreviousNode(p);
					}
				}
			}

		private:
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

			inline Node* FindNode(const K& key)
			{
				Node* p = m_pRoot;
				while (p != &m_NilNode && p->m_KeyValuePair.m_First != key)
				{
					if (LessComparer::Compare(key, p->m_KeyValuePair.m_First))
						p = p->m_pLeftChild;
					else
						p = p->m_pRightChild;
				}
				return p;
			}

			inline const Node* FindNode(const K& key) const
			{
				const Node* p = m_pRoot;
				while (p != &m_NilNode && p->m_KeyValuePair.m_First != key)
				{
					if (LessComparer::Compare(key, p->m_KeyValuePair.m_First))
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

			template<typename K2, typename V2>
			inline Pair<Node*, bool> InternalInsert(K2&& key, V2&& val)
			{
				Node* py = &m_NilNode;
				Node* px = m_pRoot;
				while (px != &m_NilNode && px->m_KeyValuePair.m_First != key)
				{
					py = px;
					if (LessComparer::Compare(key, px->m_KeyValuePair.m_First))
						px = px->m_pLeftChild;
					else
						px = px->m_pRightChild;
				}
				if (px == &m_NilNode)
				{
					Node* pz = Allocator::template New<Node>(std::forward<K2>(key), std::forward<V2>(val));
					m_Size += 1;

					pz->m_pParent = py;
					if (py == &m_NilNode)
						m_pRoot = pz;
					else if (LessComparer::Compare(pz->m_KeyValuePair.m_First, py->m_KeyValuePair.m_First))
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
					px->m_KeyValuePair.m_Second = std::forward<V2>(val);
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
				//even the pv is nil node, it also can be transplanted(see the Introduction to Algorithm).

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
			inline void CopyNode(Node* pnow, const typename RedBlackTree<K, V, LessComparer, OtherAllocator>::Node* pother, const typename RedBlackTree<K, V, LessComparer, OtherAllocator>::Node* pother_nil)
			{
				SGE_ASSERT(NullPointerError, pnow);
				SGE_ASSERT(NullPointerError, pother);
				SGE_ASSERT(NullPointerError, pother_nil);
				SGE_ASSERT(NilNodeError, pnow, &m_NilNode);
				using AnotherNilNodeError = typename SpaceGameEngine::MapImplement::RedBlackTree<K, V, LessComparer, OtherAllocator>::NilNodeError;
				SGE_ASSERT(AnotherNilNodeError, pother, pother_nil);

				if (pother->m_pLeftChild != pother_nil)
				{
					Node* pleft = Allocator::template New<Node>(pother->m_pLeftChild->m_KeyValuePair);
					pleft->m_pParent = pnow;
					pnow->m_pLeftChild = pleft;
					CopyNode<OtherAllocator>(pleft, pother->m_pLeftChild, pother_nil);
				}
				else
					pnow->m_pLeftChild = &m_NilNode;

				if (pother->m_pRightChild != pother_nil)
				{
					Node* pright = Allocator::template New<Node>(pother->m_pRightChild->m_KeyValuePair);
					pright->m_pParent = pnow;
					pnow->m_pRightChild = pright;
					CopyNode<OtherAllocator>(pright, pother->m_pRightChild, pother_nil);
				}
				else
					pnow->m_pRightChild = &m_NilNode;
			}

			template<typename OtherAllocator>
			inline void MoveNode(Node* pnow, const typename RedBlackTree<K, V, LessComparer, OtherAllocator>::Node* pother, const typename RedBlackTree<K, V, LessComparer, OtherAllocator>::Node* pother_nil)
			{
				SGE_ASSERT(NullPointerError, pnow);
				SGE_ASSERT(NullPointerError, pother);
				SGE_ASSERT(NullPointerError, pother_nil);
				SGE_ASSERT(NilNodeError, pnow, &m_NilNode);
				using AnotherNilNodeError = typename SpaceGameEngine::MapImplement::RedBlackTree<K, V, LessComparer, OtherAllocator>::NilNodeError;
				SGE_ASSERT(AnotherNilNodeError, pother, pother_nil);

				if (pother->m_pLeftChild != pother_nil)
				{
					Node* pleft = Allocator::template New<Node>(std::move(pother->m_pLeftChild->m_KeyValuePair));
					pleft->m_pParent = pnow;
					pnow->m_pLeftChild = pleft;
					MoveNode<OtherAllocator>(pleft, pother->m_pLeftChild, pother_nil);
				}
				else
					pnow->m_pLeftChild = &m_NilNode;

				if (pother->m_pRightChild != pother_nil)
				{
					Node* pright = Allocator::template New<Node>(std::move(pother->m_pRightChild->m_KeyValuePair));
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
			Node m_NilNode;
			Node* m_pRoot;
			SizeType m_Size;
		};
	}

	template<typename K, typename V, typename LessComparer = Less<K>, typename Allocator = DefaultAllocator>
	class Map
	{
	public:
		using KeyType = const K;
		using ValueType = V;
		using AllocatorType = Allocator;
		using LessComparerType = LessComparer;

		template<typename _K, typename _V, typename _LessComparer, typename _Allocator>
		friend class Map;

		inline Map()
			: m_Tree()
		{
		}

		inline ~Map()
		{
		}

		inline Map(const Map& m)
			: m_Tree(m.m_Tree)
		{
		}

		inline Map(Map&& m)
			: m_Tree(std::move(m.m_Tree))
		{
		}

		inline Map& operator=(const Map& m)
		{
			SGE_ASSERT(SelfAssignmentError, this, &m);
			m_Tree = m.m_Tree;
			return *this;
		}

		inline Map& operator=(Map&& m)
		{
			SGE_ASSERT(SelfAssignmentError, this, &m);
			m_Tree = std::move(m.m_Tree);
			return *this;
		}

		template<typename OtherAllocator>
		inline Map(const Map<K, V, LessComparer, OtherAllocator>& m)
			: m_Tree(m.m_Tree)
		{
		}

		template<typename OtherAllocator>
		inline Map(Map<K, V, LessComparer, OtherAllocator>&& m)
			: m_Tree(std::move(m.m_Tree))
		{
		}

		template<typename OtherAllocator>
		inline Map& operator=(const Map<K, V, LessComparer, OtherAllocator>& m)
		{
			m_Tree = m.m_Tree;
			return *this;
		}

		template<typename OtherAllocator>
		inline Map& operator=(Map<K, V, LessComparer, OtherAllocator>&& m)
		{
			m_Tree = std::move(m.m_Tree);
			return *this;
		}

		inline Map(std::initializer_list<Pair<const K, V>> ilist)
		{
			for (auto i = ilist.begin(); i != ilist.end(); ++i)
			{
				m_Tree.InternalInsert(i->m_First, i->m_Second);
			}
		}

		inline void Clear()
		{
			m_Tree.Clear();
		}

		inline SizeType GetSize() const
		{
			return m_Tree.GetSize();
		}

		template<typename IteratorType>
		struct IsMapIterator;

		template<typename T>
		class IteratorImpl
		{
		public:
			using ValueType = T;

			friend class Map<K, V, LessComparer, Allocator>;

			struct OutOfRangeError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("The iterator is out of range.");
				inline static bool Judge(const IteratorImpl& iter)
				{
					return iter.m_pContent == &(iter.m_pTree->m_NilNode);
				}
			};

		public:
			inline static IteratorImpl GetBegin(std::conditional_t<std::is_const_v<T>, const Map&, Map&> m)
			{
				if (m.m_Tree.m_Size)
					return IteratorImpl(m.m_Tree.GetMinimumNode(m.m_Tree.m_pRoot), &(m.m_Tree));
				else
					return IteratorImpl(&(m.m_Tree.m_NilNode), &(m.m_Tree));
			}

			inline static IteratorImpl GetEnd(std::conditional_t<std::is_const_v<T>, const Map&, Map&> m)
			{
				return IteratorImpl(&(m.m_Tree.m_NilNode), &(m.m_Tree));
			}

			inline IteratorImpl(const IteratorImpl& iter)
			{
				m_pContent = iter.m_pContent;
				m_pTree = iter.m_pTree;
			}

			inline IteratorImpl& operator=(const IteratorImpl& iter)
			{
				SGE_ASSERT(SelfAssignmentError, this, &iter);
				m_pContent = iter.m_pContent;
				m_pTree = iter.m_pTree;
				return *this;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline IteratorImpl(const IteratorType& iter)
			{
				m_pContent = (InternalPointerType)iter.m_pContent;
				m_pTree = (InternalRedBlackTreePointerType)iter.m_pTree;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline IteratorImpl& operator=(const IteratorType& iter)
			{
				m_pContent = (InternalPointerType)iter.m_pContent;
				m_pTree = (InternalRedBlackTreePointerType)iter.m_pTree;
				return *this;
			}

			inline IteratorImpl& operator++()
			{
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetNextNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMinimumNode(m_pTree->m_pRoot);
				}
				return *this;
			}

			inline const IteratorImpl operator++(int)
			{
				IteratorImpl re(*this);
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetNextNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMinimumNode(m_pTree->m_pRoot);
				}
				return re;
			}

			inline IteratorImpl& operator+=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
				{
					operator++();
				}
				return *this;
			}

			inline IteratorImpl operator+(SizeType i) const
			{
				IteratorImpl re(*this);
				re += i;
				return re;
			}

			inline IteratorImpl& operator--()
			{
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetPreviousNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMaximumNode(m_pTree->m_pRoot);
				}
				return *this;
			}

			inline const IteratorImpl operator--(int)
			{
				IteratorImpl re(*this);
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetPreviousNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMaximumNode(m_pTree->m_pRoot);
				}
				return re;
			}

			inline IteratorImpl& operator-=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
				{
					operator--();
				}
				return *this;
			}

			inline IteratorImpl operator-(SizeType i) const
			{
				IteratorImpl re(*this);
				re -= i;
				return re;
			}

			inline SizeType operator-(const IteratorImpl& iter) const
			{
				SizeType re = 0;
				IteratorImpl i = iter;
				while (i != *this)
				{
					++i;
					re += 1;
				}
				return re;
			}

			inline T* operator->() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return &(m_pContent->m_KeyValuePair);
			}

			inline T& operator*() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return m_pContent->m_KeyValuePair;
			}

			inline bool operator==(const IteratorImpl& iter) const
			{
				return m_pContent == iter.m_pContent && m_pTree == iter.m_pTree;
			}

			inline bool operator!=(const IteratorImpl& iter) const
			{
				return m_pContent != iter.m_pContent || m_pTree != iter.m_pTree;
			}

			inline T* GetData() const
			{
				return &(m_pContent->m_KeyValuePair);
			}

		private:
			using InternalPointerType = typename std::conditional_t<std::is_const_v<T>, const typename MapImplement::RedBlackTree<K, V, LessComparer, Allocator>::Node*, typename MapImplement::RedBlackTree<K, V, LessComparer, Allocator>::Node*>;
			using InternalRedBlackTreePointerType = typename std::conditional_t<std::is_const_v<T>, const MapImplement::RedBlackTree<K, V, LessComparer, Allocator>*, MapImplement::RedBlackTree<K, V, LessComparer, Allocator>*>;
			inline IteratorImpl(InternalPointerType ptr, InternalRedBlackTreePointerType ptree)
			{
				SGE_ASSERT(NullPointerError, ptr);
				SGE_ASSERT(NullPointerError, ptree);
				m_pContent = ptr;
				m_pTree = ptree;
			}

		private:
			InternalPointerType m_pContent;
			InternalRedBlackTreePointerType m_pTree;
		};

		template<typename T>
		class ReverseIteratorImpl
		{
		public:
			using ValueType = T;

			friend class Map<K, V, LessComparer, Allocator>;

			struct OutOfRangeError
			{
				inline static const TChar sm_pContent[] = SGE_TSTR("The iterator is out of range.");
				inline static bool Judge(const ReverseIteratorImpl& iter)
				{
					return iter.m_pContent == &(iter.m_pTree->m_NilNode);
				}
			};

		public:
			inline static ReverseIteratorImpl GetBegin(std::conditional_t<std::is_const_v<T>, const Map&, Map&> m)
			{
				if (m.m_Tree.m_Size)
					return ReverseIteratorImpl(m.m_Tree.GetMaximumNode(m.m_Tree.m_pRoot), &(m.m_Tree));
				else
					return ReverseIteratorImpl(&(m.m_Tree.m_NilNode), &(m.m_Tree));
			}

			inline static ReverseIteratorImpl GetEnd(std::conditional_t<std::is_const_v<T>, const Map&, Map&> m)
			{
				return ReverseIteratorImpl(&(m.m_Tree.m_NilNode), &(m.m_Tree));
			}

			inline ReverseIteratorImpl(const ReverseIteratorImpl& iter)
			{
				m_pContent = iter.m_pContent;
				m_pTree = iter.m_pTree;
			}

			inline ReverseIteratorImpl& operator=(const ReverseIteratorImpl& iter)
			{
				SGE_ASSERT(SelfAssignmentError, this, &iter);
				m_pContent = iter.m_pContent;
				m_pTree = iter.m_pTree;
				return *this;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline ReverseIteratorImpl(const IteratorType& iter)
			{
				m_pContent = (InternalPointerType)iter.m_pContent;
				m_pTree = (InternalRedBlackTreePointerType)iter.m_pTree;
			}

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value && (std::is_same_v<typename IteratorType::ValueType, ValueType> || std::is_same_v<typename IteratorType::ValueType, std::remove_const_t<ValueType>>), void>>
			inline ReverseIteratorImpl& operator=(const IteratorType& iter)
			{
				m_pContent = (InternalPointerType)iter.m_pContent;
				m_pTree = (InternalRedBlackTreePointerType)iter.m_pTree;
				return *this;
			}

			inline ReverseIteratorImpl& operator++()
			{
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetPreviousNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMaximumNode(m_pTree->m_pRoot);
				}
				return *this;
			}

			inline const ReverseIteratorImpl operator++(int)
			{
				ReverseIteratorImpl re(*this);
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetPreviousNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMaximumNode(m_pTree->m_pRoot);
				}
				return re;
			}

			inline ReverseIteratorImpl& operator+=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
				{
					operator++();
				}
				return *this;
			}

			inline ReverseIteratorImpl operator+(SizeType i) const
			{
				ReverseIteratorImpl re(*this);
				re += i;
				return re;
			}

			inline ReverseIteratorImpl& operator--()
			{
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetNextNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMinimumNode(m_pTree->m_pRoot);
				}
				return *this;
			}

			inline const ReverseIteratorImpl operator--(int)
			{
				ReverseIteratorImpl re(*this);
				if (m_pContent != &(m_pTree->m_NilNode))
					m_pContent = m_pTree->GetNextNode(m_pContent);
				else
				{
					if (m_pTree->m_Size)
						m_pContent = m_pTree->GetMinimumNode(m_pTree->m_pRoot);
				}
				return re;
			}

			inline ReverseIteratorImpl& operator-=(SizeType i)
			{
				for (SizeType j = 0; j < i; ++j)
				{
					operator--();
				}
				return *this;
			}

			inline ReverseIteratorImpl operator-(SizeType i) const
			{
				ReverseIteratorImpl re(*this);
				re -= i;
				return re;
			}

			inline SizeType operator-(const ReverseIteratorImpl& iter) const
			{
				SizeType re = 0;
				ReverseIteratorImpl i = iter;
				while (i != *this)
				{
					++i;
					re += 1;
				}
				return re;
			}

			inline T* operator->() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return &(m_pContent->m_KeyValuePair);
			}

			inline T& operator*() const
			{
				SGE_ASSERT(OutOfRangeError, *this);
				return m_pContent->m_KeyValuePair;
			}

			inline bool operator==(const ReverseIteratorImpl& iter) const
			{
				return m_pContent == iter.m_pContent && m_pTree == iter.m_pTree;
			}

			inline bool operator!=(const ReverseIteratorImpl& iter) const
			{
				return m_pContent != iter.m_pContent || m_pTree != iter.m_pTree;
			}

			inline T* GetData() const
			{
				return &(m_pContent->m_KeyValuePair);
			}

		private:
			using InternalPointerType = typename std::conditional_t<std::is_const_v<T>, const typename MapImplement::RedBlackTree<K, V, LessComparer, Allocator>::Node*, typename MapImplement::RedBlackTree<K, V, LessComparer, Allocator>::Node*>;
			using InternalRedBlackTreePointerType = typename std::conditional_t<std::is_const_v<T>, const MapImplement::RedBlackTree<K, V, LessComparer, Allocator>*, MapImplement::RedBlackTree<K, V, LessComparer, Allocator>*>;
			inline ReverseIteratorImpl(InternalPointerType ptr, InternalRedBlackTreePointerType ptree)
			{
				SGE_ASSERT(NullPointerError, ptr);
				SGE_ASSERT(NullPointerError, ptree);
				m_pContent = ptr;
				m_pTree = ptree;
			}

		private:
			InternalPointerType m_pContent;
			InternalRedBlackTreePointerType m_pTree;
		};

		using Iterator = IteratorImpl<Pair<const K, V>>;
		using ConstIterator = IteratorImpl<const Pair<const K, V>>;
		using ReverseIterator = ReverseIteratorImpl<Pair<const K, V>>;
		using ConstReverseIterator = ReverseIteratorImpl<const Pair<const K, V>>;

		template<typename IteratorType>
		struct IsMapIterator
		{
			inline static constexpr const bool Value = std::is_same_v<IteratorType, Iterator> || std::is_same_v<IteratorType, ConstIterator> || std::is_same_v<IteratorType, ReverseIterator> || std::is_same_v<IteratorType, ConstReverseIterator>;
		};

		struct ExternalIteratorError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The iterator does not belong to this Map.");

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value, void>>
			inline static bool Judge(const IteratorType& iter, const Map& m)
			{
				return iter.m_pTree != &(m.m_Tree);
			}
		};

		struct KeyNotFoundError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The key can not be found in this Map.");

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value, void>>
			inline static bool Judge(const IteratorType& iter, const IteratorType& end)
			{
				return iter == end;
			}
		};

		struct NilNodeIteratorError
		{
			inline static const TChar sm_pContent[] = SGE_TSTR("The iterator which pointing to the nil node is invalid.");

			template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value, void>>
			inline static bool Judge(const IteratorType& iter, const IteratorType& end)
			{
				return iter == end;
			}
		};

		inline Iterator GetBegin()
		{
			return Iterator::GetBegin(*this);
		}

		inline Iterator GetEnd()
		{
			return Iterator::GetEnd(*this);
		}

		inline ConstIterator GetConstBegin() const
		{
			return ConstIterator::GetBegin(*this);
		}

		inline ConstIterator GetConstEnd() const
		{
			return ConstIterator::GetEnd(*this);
		}

		inline ReverseIterator GetReverseBegin()
		{
			return ReverseIterator::GetBegin(*this);
		}

		inline ReverseIterator GetReverseEnd()
		{
			return ReverseIterator::GetEnd(*this);
		}

		inline ConstReverseIterator GetConstReverseBegin() const
		{
			return ConstReverseIterator::GetBegin(*this);
		}

		inline ConstReverseIterator GetConstReverseEnd() const
		{
			return ConstReverseIterator::GetEnd(*this);
		}

		template<typename K2, typename V2>
		inline Pair<Iterator, bool> Insert(K2&& key, V2&& val)
		{
			auto re = m_Tree.InternalInsert(std::forward<K2>(key), std::forward<V2>(val));
			return Pair<Iterator, bool>(Iterator(re.m_First, &m_Tree), re.m_Second);
		}

		inline void Insert(std::initializer_list<Pair<const K, V>> ilist)
		{
			for (auto i = ilist.begin(); i != ilist.end(); ++i)
			{
				m_Tree.InternalInsert(i->m_First, i->m_Second);
			}
		}

		template<typename IteratorType, typename = std::enable_if_t<IsMapIterator<IteratorType>::Value, void>>
		inline IteratorType Remove(const IteratorType& iter)
		{
			SGE_ASSERT(ExternalIteratorError, iter, *this);
			SGE_ASSERT(NilNodeIteratorError, iter, IteratorType::GetEnd(*this));
			IteratorType re = iter;
			++re;
			m_Tree.RemoveNode((typename MapImplement::RedBlackTree<K, V, LessComparer, Allocator>::Node*)iter.m_pContent);
			return re;
		}

		inline bool RemoveByKey(const K& key)
		{
			return m_Tree.RemoveByKey(key);
		}

		inline Iterator Find(const K& key)
		{
			return Iterator(m_Tree.FindNode(key), &m_Tree);
		}

		inline ConstIterator Find(const K& key) const
		{
			return ConstIterator(m_Tree.FindNode(key), &m_Tree);
		}

		inline V& Get(const K& key)
		{
			Iterator iter = Find(key);
			SGE_CHECK(KeyNotFoundError, iter, GetEnd());
			return iter->m_Second;
		}

		inline const V& Get(const K& key) const
		{
			ConstIterator iter = Find(key);
			SGE_CHECK(KeyNotFoundError, iter, GetConstEnd());
			return iter->m_Second;
		}

		template<typename K2>
		inline V& operator[](K2&& key)
		{
			Iterator iter = Find(key);
			if (iter == GetEnd())
				iter = Insert(std::forward<K2>(key), V()).m_First;
			return iter->m_Second;
		}

	private:
		MapImplement::RedBlackTree<K, V, LessComparer, Allocator> m_Tree;
	};
	/*!
	@}
	*/
}