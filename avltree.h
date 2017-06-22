#ifndef AVL_HEAD_H_2015_03_31
#define AVL_HEAD_H_2015_03_31

#include "memorymanager.h"

namespace tree {

    template<class T, int(*Compare)(const T *pElement, const T *pElement2)>
    class CAVLTree {
    private:
        struct leaf {
            T *pData;
            leaf *pLeft;
            leaf *pRight;
            int balanceFactor;

            leaf(T *data) : pData(data), pLeft(0), pRight(0), balanceFactor(1) { }

            leaf() { }
        };

    public:
        class CException {
        public:
            CException() {
            }
        };

    public:
        CAVLTree() : m_Memory(128, true), m_pRoot(0) { }

        virtual ~CAVLTree() {
            delete (m_pRoot);
        }

        bool add(T *pElement) {
            if (m_pRoot == 0) {
                leaf *pNew = m_Memory.newObject();
                *pNew = leaf(pElement);
                m_pRoot = new leaf *(pNew);
                return true;
            } else {
                bool added = false;
                insert(*m_pRoot, pElement, added);
                return added;
            }
        }

        bool update(T *pElement) {
            if (m_pRoot == 0) {
                return 0;
            } else {
                return updateFrom(*m_pRoot, pElement);
            }
        }

        T *find(const T &pElement) {
            if (m_pRoot == 0) {
                return 0;
            } else {
                return findFrom(*m_pRoot, pElement);
            }
        }

        bool remove(const T &element) {
            if (m_pRoot == 0) {
                return false;
            }
            bool removed = false;
            removeFrom(*m_pRoot, element, removed);
            return removed;
        }

        void clear() {
            m_Memory.clear();
        }

    private:
        leaf **m_pRoot;
        Memory::CMemoryManager<leaf> m_Memory;

        int bfactor(leaf *p) {
            return balanceFactor(p->pRight) - balanceFactor(p->pLeft);
        }

        int balanceFactor(leaf *p) {
            if (p) {
                return p->balanceFactor;
            } else {
                return 0;
            }
        }

        leaf *rotateRight(leaf *p) {
            leaf *q = p->pLeft;
            p->pLeft = q->pRight;
            q->pRight = p;
            fixbalanceFactor(p);
            fixbalanceFactor(q);

            if (*m_pRoot == q->pRight || *m_pRoot == q->pLeft) {
                *m_pRoot = q;
            }
            return q;
        }

        leaf *rotateLeft(leaf *q) {
            leaf *p = q->pRight;
            q->pRight = p->pLeft;
            p->pLeft = q;
            fixbalanceFactor(q);
            fixbalanceFactor(p);
            if (*m_pRoot == p->pRight || *m_pRoot == p->pLeft) {
                *m_pRoot = p;
            }
            return p;
        }

        void fixbalanceFactor(leaf *p) {
            int balanceLeft = balanceFactor(p->pLeft);
            int balanceRight = balanceFactor(p->pRight);
            if (balanceLeft > balanceRight) {
                p->balanceFactor = balanceLeft;
            } else {
                p->balanceFactor = balanceRight;
            }
            ++p->balanceFactor;
        }

        leaf *balance(leaf *p) {
            fixbalanceFactor(p);
            if (bfactor(p) == 2) {
                if (bfactor(p->pRight) < 0) {
                    p->pRight = rotateRight(p->pRight);
                }
                return rotateLeft(p);
            }
            if (bfactor(p) == -2) {
                if (bfactor(p->pLeft) > 0) {
                    p->pLeft = rotateLeft(p->pLeft);
                }
                return rotateRight(p);
            }
            return p;
        }

        leaf *findMinimum(leaf *p) {
            if (p->pLeft) {
                return findMinimum(p->pLeft);
            } else {
                return p;
            }
        }

        leaf *insert(leaf *p, T *k, bool &added) {
            if (p == 0) {
                leaf *pleaf = m_Memory.newObject();
                *pleaf = leaf(k);
                added = true;
                return pleaf;
            }
            if (Compare(k, p->pData) < 0) {
                p->pLeft = insert(p->pLeft, k, added);
            } else if (Compare(k, p->pData) > 0) {
                p->pRight = insert(p->pRight, k, added);
            } else {
                added = false;
            }
            return balance(p);
        }

        leaf *removeMinimum(leaf *p) {
            if (p->pLeft == 0) {
                return p->pRight;
            }
            p->pLeft = removeMinimum(p->pLeft);
            return balance(p);
        }

        leaf *removeFrom(leaf *p, const T &k, bool &removed) {
            if (p == 0) {
                return 0;
            }
            if (Compare(&k, p->pData) < 0) {
                p->pLeft = removeFrom(p->pLeft, k, removed);
            }
            else if (Compare(&k, p->pData) > 0) {
                p->pRight = removeFrom(p->pRight, k, removed);
            } else {
                if ((*m_pRoot)->pRight == 0 && (*m_pRoot)->pLeft == 0) {
                    delete m_pRoot;
                    removed = true;
                    m_pRoot = 0;
                    return 0;
                }
                leaf *q = p->pLeft;
                leaf *r = p->pRight;
                bool updateRootNeeded = false;
                if (*m_pRoot == p) {
                    updateRootNeeded = true;
                }
                if (p != *m_pRoot) {
                    m_Memory.deleteObject(p);
                }
                removed = true;
                if (r == 0) {
                    if (updateRootNeeded) {
                        *m_pRoot = q;
                    }
                    return q;
                }
                leaf *min = findMinimum(r);
                min->pRight = removeMinimum(r);
                min->pLeft = q;
                leaf *res = balance(min);
                if (updateRootNeeded) {
                    *m_pRoot = res;
                }
                return res;
            }
            return balance(p);
        }

        bool updateFrom(leaf *p, T *pElement) {
            if (p == 0) {
                return false;
            }
            if (Compare(pElement, p->pData) < 0) {
                return updateFrom(p->pLeft, pElement);
            }
            else if (Compare(pElement, p->pData) > 0) {
                return updateFrom(p->pRight, pElement);
            } else {
                p->pData = pElement;
                return true;
            }
        }

        T *findFrom(leaf *p, const T &element) {
            if (p == 0) {
                return 0;
            }
            if (Compare(&element, p->pData) < 0) {
                return findFrom(p->pLeft, element);
            }
            else if (Compare(&element, p->pData) > 0) {
                return findFrom(p->pRight, element);
            } else {
                return p->pData;
            }
        }
    };

}; // namespace templates

#endif // #define AVL_HEAD_H_2015_03_31
