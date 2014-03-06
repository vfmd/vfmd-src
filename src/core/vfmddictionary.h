#ifndef VFMDDICTIONARY_H
#define VFMDDICTIONARY_H

// VfmdDictionary is an associative array, with keys as bytearrays.

#include <assert.h>
#include <stdio.h>
#include "vfmdbytearray.h"

extern "C" {
#include "red_black_tree.h"
}

static int compareByteArrays(const void *ba_ptr1, const void *ba_ptr2);
static void destroyByteArray(void *ba_ptr);
static void printConstByteArray(const void *ba_ptr);
static void notPrintingVoidPtr(void *ptr);

template<class T>
class VfmdDictionary
{
public:
    VfmdDictionary() {
        m_redBlackTree = RBTreeCreate(&compareByteArrays,
                                      &destroyByteArray, &VfmdDictionary<T>::destroyValueObject,
                                      &printConstByteArray, &notPrintingVoidPtr);
    }

    ~VfmdDictionary() {
        RBTreeDestroy(m_redBlackTree);
    }

    /* Insert key-value pair even if 'key' is already in the dictionary */
    bool insertMulti(const VfmdByteArray &key, const T *value) {
        add(key, value);
        return true;
    }

    /* Insert key-value pair. If 'key' already exists, replace the value. */
    void replace(const VfmdByteArray &key, const T *value) {
        rb_red_blk_node *node = lookup(key);
        if (node) {
            // Key found, replace value
            destroyValueObject(node->info);
            node->info = value;
        } else {
            // Key not found, add key-value pair
            add(key, value);
        }
    }

    /* Look-up the value for a key. */
    T* value(const VfmdByteArray& key) {
        rb_red_blk_node *node = lookup(key);
        if (node) {
            T *obj = static_cast<T *>(node->info);
            return obj;
        }
        return 0;
    }

    /* Check if a key exists. */
    bool contains(const VfmdByteArray &key) {
        rb_red_blk_node *node = lookup(key);
        if (node) {
            return true;
        }
        return false;
    }

    /* Remove a key-value pair. Returns 'true' if removed. Returns 'false' if key
     * was not found. */
    bool remove(const VfmdByteArray &key) {
        rb_red_blk_node *node = lookup(key);
        if (node) {
            deleteNode(node);
            return true;
        }
        return false;
    }

    /* Print the keys in the dictionary for debugging */
    void printKeys() const {
        printDictKeys();
    }

    static void destroyValueObject(void *ptr) {
        T *obj = static_cast<T *>(ptr);
        delete obj;
    }

private:
    /* Prevent copying of this class */
    VfmdDictionary(const VfmdDictionary& other);
    VfmdDictionary& operator=(const VfmdDictionary& other);

    rb_red_blk_node* add(const VfmdByteArray &key, const T *value) {
        return RBTreeInsert(m_redBlackTree, key.copy(), (void *) value);
    }

    rb_red_blk_node* lookup(const VfmdByteArray &key) const {
        return RBExactQuery(m_redBlackTree, (void *) &key);
    }

    void deleteNode(rb_red_blk_node* node) {
        RBDelete(m_redBlackTree, node);
    }

    void printDictKeys() const {
        RBTreePrint(m_redBlackTree);
    }

    rb_red_blk_tree* m_redBlackTree;
};

static int compareByteArrays(const void *ba_ptr1, const void *ba_ptr2)
{
    const VfmdByteArray *ba1 = static_cast<const VfmdByteArray *>(ba_ptr1);
    const VfmdByteArray *ba2 = static_cast<const VfmdByteArray *>(ba_ptr2);

    for (unsigned int i = 0; i < ba1->size() && i < ba2->size(); i++) {
        if (ba1->byteAt(i) < ba2->byteAt(i)) {
            return -1;
        } else if (ba1->byteAt(i) > ba2->byteAt(i)) {
            return 1;
        } else {
            // equal
        }
    }

    if (ba1->size() < ba2->size()) {
        return -1;
    } else if (ba1->size() > ba2->size()) {
        return 1;
    }
    return 0;
}

static void destroyByteArray(void *ba_ptr)
{
    const VfmdByteArray *ba = static_cast<VfmdByteArray *>(ba_ptr);
    delete ba;
}

static void printConstByteArray(const void *ba_ptr)
{
    const VfmdByteArray *ba = static_cast<const VfmdByteArray *>(ba_ptr);
    ba->print();
}

static void notPrintingVoidPtr(void *ptr)
{
    (void) ptr;
}

#endif // VFMDDICTIONARY_H
