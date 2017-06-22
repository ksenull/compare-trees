#ifndef MEMORY_MANAGER_HEAD_H_2015_03_06
#define MEMORY_MANAGER_HEAD_H_2015_03_06

namespace Memory // TODO поменять на свое название
{

    template<class T>
    class CMemoryManager {
    private:
        struct block {
            T *pdata;
            block *pnext;
            int firstFreeIndex;
            int usedCount;
        };
    public:
        class CException {
        public:
            CException() {
            }
        };

    public:
        CMemoryManager(int _default_block_size, bool isDeleteElementsOnDestruct = false) {
            m_pBlocks = 0;
            m_pCurrentBlk = 0;
            m_blkSize = _default_block_size;
            m_isDeleteElementsOnDestruct = isDeleteElementsOnDestruct;
        }

        virtual ~CMemoryManager() {
            this->clear();
        }

        T *newObject() {
            if (m_pCurrentBlk == 0) {
                m_pBlocks = newBlock();
                m_pCurrentBlk = m_pBlocks;
            }

            if (m_pCurrentBlk->firstFreeIndex == -1) {
                block *pBlock = m_pBlocks;
                block *pPrev = 0;

                while (pBlock != 0 && pBlock->firstFreeIndex == -1) {
                    pPrev = pBlock;
                    pBlock = pBlock->pnext;
                }

                if (pBlock != 0) {
                    m_pCurrentBlk = pBlock;
                } else {
                    m_pCurrentBlk = newBlock();
                    pPrev->pnext = m_pCurrentBlk;
                }
            }

            int i = m_pCurrentBlk->firstFreeIndex;
            m_pCurrentBlk->firstFreeIndex = *(reinterpret_cast<int *>((m_pCurrentBlk->pdata) + i));

            new(m_pCurrentBlk->pdata + i) T;
            m_pCurrentBlk->usedCount++;
            return m_pCurrentBlk->pdata + i;
        }

        bool deleteObject(T *p) {
            block *pBlock = m_pBlocks;

            while (pBlock != 0 && (pBlock->pdata > p || pBlock->pdata + m_blkSize - 1 < p)) {
                pBlock = pBlock->pnext;
            }

            if (pBlock == 0) {
                return false;
            }

            int i = p - pBlock->pdata;
            p->~T();
            pBlock->usedCount--;
            *(reinterpret_cast<int *>(p)) = pBlock->firstFreeIndex;
            pBlock->firstFreeIndex = i;
            return true;
        }

        void clear() {
            if (m_pBlocks == 0) {
                return;
            }
            if (!m_isDeleteElementsOnDestruct) {
                block *pBlock = m_pBlocks;

                while (pBlock != 0) {
                    if (pBlock->usedCount != 0) {
                        throw CException();
                    }
                    pBlock = pBlock->pnext;
                }

                pBlock = m_pBlocks;
                block *pNext = 0;

                while (pBlock != 0) {
                    pNext = pBlock->pnext;
                    delete[] pBlock->pdata;
                    delete pBlock;
                    pBlock = pNext;
                }
            } else {
                block *pBlock = m_pBlocks;
                block *pNext = 0;

                while (pBlock != 0) {
                    pNext = pBlock->pnext;
                    deleteBlock(pBlock);
                    pBlock = pNext;
                }

            }
            m_pCurrentBlk = 0;
            m_pBlocks = 0;

        }

    private:
        block *newBlock() {
            block *pBlock = new block;

            pBlock->firstFreeIndex = 0;
            pBlock->pnext = 0;
            pBlock->usedCount = 0;

            pBlock->pdata = reinterpret_cast<T *>(new char[m_blkSize * sizeof(T)]);
            T *pBlockData = pBlock->pdata;

            for (int i = 0; i < m_blkSize - 1; ++i) {
                *(reinterpret_cast<int *>(pBlockData + i)) = i + 1;
            }

            *(reinterpret_cast<int *>(pBlockData + m_blkSize - 1)) = -1;
            return pBlock;
        }

        void deleteBlock(block *p) {
            bool *isDelete = new bool[m_blkSize];

            for (int i = 0; i < m_blkSize; ++i) {
                isDelete[i] = true;
            }

            T *pBlockData = p->pdata;
            int i = p->firstFreeIndex;

            while (i != -1) {
                isDelete[i] = false;
                i = *(reinterpret_cast<int *>(pBlockData + i));
            }

            for (int i = 0; i < m_blkSize; ++i) {
                if (isDelete[i]) {
                    (pBlockData + i)->~T();
                }
            }

            delete[] pBlockData;
            delete[] isDelete;
            delete p;
        }

        int m_blkSize;
        block *m_pBlocks;
        block *m_pCurrentBlk;
        bool m_isDeleteElementsOnDestruct;
    };

}; // namespace templates

#endif // #define MEMORY_MANAGER_HEAD_H_2015_03_06