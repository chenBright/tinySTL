#ifndef TINYSTL_RB_TREE_H
#define TINYSTL_RB_TREE_H

#include <cstddef>

#include <type_traits>
#include <initializer_list>
#include <limits>

#include "type_traits.h"
#include "iterator_base.h"
#include "iterator_reverse_iterator.h"
#include "alloc.h"
#include "constrcut.h"
#include "utility_pair.h"
#include "utility_move.h"

// rb_tree 红黑树
// 实现来自：https://github.com/liuyunbin/tiny-STL/blob/1dd6633ccd25f2a58465a0d528fb6c1bbfe9c412/src/stl_rb_tree.h

namespace tinySTL {
    /********************************************
     *                                          *
     *             rb_tree_node                 *
     *                                          *
     ********************************************/

    using rb_tree_color_type = bool;
    constexpr rb_tree_color_type rb_tree_black =  true;
    constexpr rb_tree_color_type rb_tree_red   = false;

    template <typename T>
    struct rb_tree_node {
        rb_tree_color_type color_;
        rb_tree_node* left_;
        rb_tree_node* right_;
        rb_tree_node* parent_;

        T data_;
    };

    /********************************************
     *                                          *
     *             rb_tree_iterator             *
     *                                          *
     ********************************************/

    template <typename T, typename Ref, typename Ptr>
    struct rb_tree_iterator {
        using        value_type = T;
        using         reference = Ref;
        using           pointer = Ptr;
        using   difference_type = std::ptrdiff_t;
        using iterator_category = bidirectional_iterator_tag;

        using       iterator = rb_tree_iterator<T,       T&,       T*>;
        using const_iterator = rb_tree_iterator<T, const T&, const T*>;
        using  self_iterator = rb_tree_iterator<T,      Ref,      Ptr>;

        rb_tree_node<T>* node_;

        rb_tree_iterator()                            : node_(nullptr)     {}
        explicit rb_tree_iterator(      rb_tree_node<T>* node) : node_(node)        {}
        explicit rb_tree_iterator(const iterator& other)       : node_(other.node_) {}

        reference operator* () const { return   node_->data_; }
        pointer   operator->() const { return &(operator*()); }

        self_iterator& operator= (const iterator& other) { node_ = other.node_; return *this; }

        self_iterator& operator++() {
            if (node_->right_ != nullptr) {
                node_ = node_->right_;
                while (node_->left_ != nullptr)
                    node_ = node_->left_;
                return *this;
            }
            auto parent = node_->parent_;
            while (parent->right_ == node_) {
                node_ = parent;
                parent = node_->parent_;
            }
            if (node_->right_ != parent) // if node_ == header_ nothing, else node_ = parent
                node_ = parent;
            return *this;
        }

        self_iterator operator++(int) {
            auto temp = *this;
            ++*this;
            return temp;
        }

        self_iterator& operator--() {
            if (node_->color_ == rb_tree_red && node_->parent_->parent_ == node_) { // node_ == header_
                node_ = node_->right_;
                return *this;
            }
            if (node_->left_ != nullptr) {
                node_ = node_->left_;
                while (node_->right_ != nullptr)
                    node_ = node_->right_;
                return *this;
            }
            auto parent = node_->parent_;
            while (parent->left_ == node_) {
                node_ = parent;
                parent = node_->parent_;
            }
            node_ = parent;
            return *this;
        }

        self_iterator operator--(int) {
            auto temp = *this;
            --*this;
            return temp;
        }

        bool operator==(const       iterator& right) const { return node_ == right.node_; }
        bool operator==(const const_iterator& right) const { return node_ == right.node_; }
        bool operator!=(const       iterator& right) const { return node_ != right.node_; }
        bool operator!=(const const_iterator& right) const { return node_ != right.node_; }
    };

    /************************************************************
     *                                                          *
     *                rb_tree_rotate_left                       *
     *                                                          *
     *                rb_tree_rotate_right                      *
     *                                                          *
     *                rb_tree_reblance                          *
     *                                                          *
     *                rb_tree_rebalance_for_erase               *
     *                                                          *
     ************************************************************/

    template <typename T>
    void rb_tree_rotate_left(rb_tree_node<T>* x, rb_tree_node<T>*& root) {
        auto y    = x->right_;
        x->right_ = y->left_;
        if (y->left_ != nullptr)
            y->left_->parent_ = x;
        y->parent_ = x->parent_;
        if (x == root)
            root = y;
        else if (x == x->parent_->left_)
            x->parent_->left_  = y;
        else
            x->parent_->right_ = y;
        y->left_   = x;
        x->parent_ = y;
    }

    template <typename T>
    void rb_tree_rotate_right(rb_tree_node<T>* x, rb_tree_node<T>*& root) {
        auto y   = x->left_;
        x->left_ = y->right_;
        if (y->right_ != nullptr)
            y->right_->parent_ = x;
        y->parent_ = x->parent_;
        if (x == root)
            root = y;
        else if (x == x->parent_->left_)
            x->parent_->left_  = y;
        else
            x->parent_->right_ = y;
        y->right_  = x;
        x->parent_ = y;
    }

    template <typename T>
    void rb_tree_rebalance(rb_tree_node<T>* x, rb_tree_node<T>*& root) {
        x->color_ = rb_tree_red;
        while (x != root && x->parent_->color_ == rb_tree_red) {
            if (x->parent_ == x->parent_->parent_->left_) {
                auto uncle = x->parent_->parent_->right_;
                if (uncle != nullptr && uncle->color_ == rb_tree_red) {
                    uncle->color_ = x->parent_->color_ = rb_tree_black;
                    x->parent_->parent_->color_ = rb_tree_red;
                    x = x->parent_->parent_;
                } else {
                    if (x == x->parent_->right_) {
                        x = x->parent_;
                        rb_tree_rotate_left(x, root);
                    }
                    x->parent_->color_          = rb_tree_black;
                    x->parent_->parent_->color_ = rb_tree_red;
                    rb_tree_rotate_right(x->parent_->parent_, root);
                }
            } else {
                auto uncle = x->parent_->parent_->left_;
                if (uncle != nullptr && uncle->color_ == rb_tree_red) {
                    uncle->color_ = x->parent_->color_ = rb_tree_black;
                    x->parent_->parent_->color_ = rb_tree_red;
                    x = x->parent_->parent_;
                } else {
                    if (x == x->parent_->left_) {
                        x = x->parent_;
                        rb_tree_rotate_right(x, root);
                    }
                    x->parent_->color_          = rb_tree_black;
                    x->parent_->parent_->color_ = rb_tree_red;
                    rb_tree_rotate_left(x->parent_->parent_, root);
                }
            }
        }
        root->color_ = rb_tree_black;
    }

    template <typename T>
    rb_tree_node<T>* rb_tree_rebalance_for_erase(rb_tree_node<T>*  z,
                                                 rb_tree_node<T>*& root,
                                                 rb_tree_node<T>*&  left_most,
                                                 rb_tree_node<T>*& right_most) {

        auto y = z;
        decltype(z) x;
        decltype(z) x_parent;
        if (y->left_ == nullptr)
            x = y->right_;
        else if (y->right_ == nullptr)
            x = y->left_;
        else {
            y = y->right_;
            while (y->left_ != nullptr)
                y = y->left_;
            x = y->right_;
        }

        if (y != z) {
            y->left_          = z->left_;
            y->left_->parent_ = y;
            if (y != z->right_) {
                x_parent = y->parent_;
                if (x != nullptr)
                    x->parent_ = x_parent;
                y->parent_->left_  = x;
                y->right_          = z->right_;
                y->right_->parent_ = y;
            } else {
                x_parent = y;
            }
            y->parent_ = z->parent_;
            if (z == root)
                root = y;
            else if (z->parent_->left_ == z)
                y->parent_->left_  = y;
            else
                y->parent_->right_ = y;
            using std::swap;
            swap(z->color_, y->color_);
            y = z;
        } else {
            x_parent = y->parent_;
            if (x != nullptr)
                x->parent_ = x_parent;
            if (z == root)
                root = x;
            else if (z == z->parent_->left_)
                z->parent_->left_  = x;
            else
                z->parent_->right_ = x;
            if (left_most == z) {
                if (z->right_ == nullptr) {
                    left_most = z->parent_;
                }
                else {
                    left_most = z->right_;
                    while (left_most->left_ != nullptr)
                        left_most = left_most->left_;
                }
            }
            if (right_most == z) {
                if (z->left_ == nullptr) {
                    right_most = z->parent_;
                }
                else {
                    right_most = z->left_;
                    while (right_most->right_ != nullptr)
                        right_most = right_most->right_;
                }
            }
        }

        if (y->color_ == rb_tree_red)
            return y;

        while (x != root && (x == nullptr || x->color_ == rb_tree_black)) {
            if (x == x_parent->left_) {
                auto w = x_parent->right_;
                if (w->color_ == rb_tree_red) {
                    w->color_ = rb_tree_black;
                    x_parent->color_ = rb_tree_red;
                    rb_tree_rotate_left(x_parent, root);
                    w = x_parent->right_;
                } // now x's brother's color is black (w)
                if ((w->left_  == nullptr || w->left_->color_  == rb_tree_black) &&
                    (w->right_ == nullptr || w->right_->color_ == rb_tree_black)) {

                    w->color_ = rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent_;
                } else {
                    if (w->right_ == nullptr || w->right_->color_ == rb_tree_black) {
                        w->left_->color_ = rb_tree_black;
                        w->color_ = rb_tree_red;
                        rb_tree_rotate_right(w, root);
                        w = x_parent->right_;
                    } // now w has a red right child
                    w->color_ = x_parent->color_;
                    x_parent->color_ = rb_tree_black;
                    w->right_->color_ = rb_tree_black;
                    rb_tree_rotate_left(x_parent, root);
                    break;
                }
            } else {
                auto w = x_parent->left_;
                if (w->color_ == rb_tree_red) {
                    w->color_ = rb_tree_black;
                    x_parent->color_ = rb_tree_red;
                    rb_tree_rotate_right(x_parent, root);
                    w = x_parent->left_;
                }
                if ((w->left_  == nullptr || w->left_->color_  == rb_tree_black) &&
                    (w->right_ == nullptr || w->right_->color_ == rb_tree_black)) {

                    w->color_ = rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent_;
                } else {
                    if (w->left_ == nullptr || w->left_->color_ == rb_tree_black) {
                        w->right_->color_ = rb_tree_red;
                        w->color_ = rb_tree_red;
                        rb_tree_rotate_left(w, root);
                        w = x_parent->left_;
                    }
                    w->color_ = x_parent->color_;
                    x_parent->color_ = rb_tree_black;
                    w->left_->color_ = rb_tree_black;
                    rb_tree_rotate_right(x_parent, root);
                    break;
                }
            }
        }
        if (x != nullptr)
            x->color_ = rb_tree_red;
        return y;
    }

    /********************************************
     *                                          *
     *                  rb_tree                 *
     *                                          *
     ********************************************/

    template <typename Key, typename Value, typename KeyOfValue, typename Compare, typename Allocator = tinySTL::allocator<Value>>
    class rb_tree {
    public:

        using                key_type = Key;
        using              value_type = Value;
        using          allocator_type = Allocator;
        using               size_type = std::size_t;
        using         difference_type = std::ptrdiff_t;
        using               reference =       value_type&;
        using         const_reference = const value_type&;
        using                 pointer =       value_type*;
        using           const_pointer = const value_type*;

        using                iterator = rb_tree_iterator<value_type,       reference,       pointer>;
        using          const_iterator = rb_tree_iterator<value_type, const_reference, const_pointer>;
        using        reverse_iterator = tinySTL::reverse_iterator<      iterator>;
        using  const_reverse_iterator = tinySTL::reverse_iterator<const_iterator>;

        /***********************************
         *                                 *
         *         constructor             *
         *                                 *
         ***********************************/

        rb_tree(const Compare& comp = Compare()) :count_node_(0), comp_(comp) {
            header_ = allocate_node();
            header_->left_   = header_->right_ = header_;
            header_->parent_ = nullptr;
            header_->color_  = rb_tree_red;
        }

        rb_tree(      rb_tree&& other) { header_ = nullptr; move_from(other); }
        rb_tree(const rb_tree&  other) { header_ = nullptr; copy_from(other); }

        /**************************************
         *                                    *
         *           destructor               *
         *                                    *
         **************************************/

        ~rb_tree() { delete_rb_tree(); }


        /***********************************
         *                                 *
         *         begin     end           *
         *                                 *
         *        cbegin    cend           *
         *                                 *
         ***********************************/

        iterator  begin()       { return static_cast<      iterator>(header_->left_); }
        const_iterator  begin() const { return static_cast<const_iterator>(header_->left_); }
        const_iterator cbegin() const { return static_cast<const_iterator>(header_->left_); }

        iterator    end()       { return static_cast<      iterator>(header_);    }
        const_iterator    end() const { return static_cast<const_iterator>(header_);    }
        const_iterator   cend() const { return static_cast<const_iterator>(header_);    }

        /***********************************
         *                                 *
         *         rbegin     rend         *
         *                                 *
         *        crbegin    crend         *
         *                                 *
         ***********************************/

        reverse_iterator  rbegin()       { return static_cast<      reverse_iterator>(   end()); }
        const_reverse_iterator  rbegin() const { return static_cast<const_reverse_iterator>(   end()); }
        const_reverse_iterator crbegin() const { return static_cast<const_reverse_iterator>(  cend()); }

        reverse_iterator    rend()       { return static_cast<      reverse_iterator>( begin()); }
        const_reverse_iterator    rend() const { return static_cast<const_reverse_iterator>( begin()); }
        const_reverse_iterator   crend() const { return static_cast<const_reverse_iterator>(cbegin()); }

        /************************************
         *                                  *
         *             size                 *
         *                                  *
         *             empty                *
         *                                  *
         *             max_size             *
         *                                  *
         *             key_comp             *
         *                                  *
         ************************************/

        size_type        size() const { return count_node_;                           }

        bool            empty() const { return size() == 0;                           }

        size_type    max_size() const { return std::numeric_limits<size_type>::max(); }

        Compare      key_comp() const { return comp_;                                 }

        /*************************************
         *                                   *
         *               =                   *
         *                                   *
         *************************************/

        rb_tree& operator= (      rb_tree&& other) {
            if (this != &other)
                move_from(other);
            return *this;
        }

        rb_tree& operator= (const rb_tree& other) {
            if (this != &other)
                copy_from(other);
            return *this;
        }

        /*************************************
         *                                   *
         *             find                  *
         *                                   *
         *************************************/

        iterator find(const key_type& k) {
            auto y = header_;
            auto x = header_->parent_;
            while (x != nullptr) {
                if (!comp_(key(x), k)) { // >=
                    y = x;
                    x = x->left_;
                }
                else
                    x = x->right_;
            }
            auto it = static_cast<iterator>(y);
            return (it == end() || comp_(key(y), k)) ? end() : it;
        }

        const_iterator find(const key_type& k) const {
            auto y = header_;
            auto x = header_->parent_;
            while (x != nullptr) {
                if (!comp_(key(x), k)) { // >=
                    y = x;
                    x = x->left_;
                }
                else
                    x = x->right_;
            }
            auto it = static_cast<const_iterator>(y);
            return (it == end() || comp_(key(y), k)) ? end() : it;
        }

        /*************************************
         *                                   *
         *           equal_range             *
         *                                   *
         *           lower_bound             *
         *                                   *
         *           upper_bound             *
         *                                   *
         *************************************/

        tinySTL::pair<      iterator,       iterator> equal_range(const key_type& k)       { return { lower_bound(k), upper_bound(k) }; }
        tinySTL::pair<const_iterator, const_iterator> equal_range(const key_type& k) const { return { lower_bound(k), upper_bound(k) }; }

        iterator lower_bound(const key_type& k) {
            auto y = header_;
            auto x = header_->parent_;
            while (x != nullptr) {
                if (!comp_(key(x), k)) { // >=
                    y = x;
                    x = x->left_;
                }
                else
                    x = x->right_;
            }
            return static_cast<iterator>(y);
        }

        const_iterator lower_bound(const key_type& k) const {
            auto y = header_;
            auto x = header_->parent_;
            while (x != nullptr) {
                if (!comp_(key(x), k)) { // >=
                    y = x;
                    x = x->left_;
                }
                else
                    x = x->right_;
            }
            return static_cast<const_iterator>(y);
        }

        iterator upper_bound(const key_type& k) {
            auto y = header_;
            auto x = header_->parent_;
            while (x != nullptr) {
                if (comp_(k, key(x))) { // key(x) > k
                    y = x;
                    x = x->left_;
                }
                else
                    x = x->right_;
            }
            return static_cast<iterator>(y);
        }

        const_iterator upper_bound(const key_type& k) const {
            auto y = header_;
            auto x = header_->parent_;
            while (x != nullptr) {
                if (comp_(k, key(x))) { // key(x) > k
                    y = x;
                    x = x->left_;
                }
                else
                    x = x->right_;
            }
            return static_cast<const_iterator>(y);
        }

        /*************************************
         *                                   *
         *          count                    *
         *                                   *
         *************************************/

        size_type count(const key_type& k) const { return tinySTL::distance(lower_bound(k), upper_bound(k)); }

        /*************************************
         *                                   *
         *           insert_equal            *
         *                                   *
         *          emplace_equal            *
         *                                   *
         *           insert_unique           *
         *                                   *
         *          emplace_unique           *
         *                                   *
         *************************************/

        iterator insert_equal(const value_type&  value) { return insert_equal_aux(value);                }
        iterator insert_equal(      value_type&& value) { return insert_equal_aux(tinySTL::move(value)); }

        template <typename InputIt>
        void insert_equal(InputIt first, InputIt last) {
            while (first != last)
                insert_equal(*first++);
        }

        void insert_equal(std::initializer_list<value_type> ilist) {
            for (auto it = ilist.begin(); it != ilist.end(); ++it)
                insert_equal(*it);
        }

        template <typename... Args>
        iterator emplace_equal(Args&&... args) { return insert_equal(value_type(std::forward<Args>(args)...)); }

        tinySTL::pair<iterator, bool> insert_unique(const value_type&  value) { return insert_unique_aux(value);                 }
        tinySTL::pair<iterator, bool> insert_unique(      value_type&& value) { return insert_unique_aux(tinySTL::move(value));  }

        template <typename InputIt>
        void insert_unique(InputIt first, InputIt last) {
            while (first != last)
                insert_unique(*first++);
        }

        void insert_unique(std::initializer_list<value_type> ilist) {
            for (auto it = ilist.begin(); it != ilist.end(); ++it)
                insert_unique(*it);
        }

        template <typename... Args>
        tinySTL::pair<iterator, bool> emplace_unique(Args&&... args) { return insert_unique(value_type(std::forward<Args>(args)...)); }

        /********************************************
         *                                          *
         *               erase                      *
         *                                          *
         *               clear                      *
         *                                          *
         ********************************************/

        iterator erase(const_iterator pos) {
            auto return_iterator = static_cast<iterator>(pos.node_);
            ++return_iterator;
            --count_node_;

            auto temp = rb_tree_rebalance_for_erase(pos.node_, header_->parent_, header_->left_, header_->right_);
            destroy_node(temp);

            return return_iterator;
        }

        iterator erase(const_iterator first, const_iterator last) {
            while (first != last)
                first = erase(first);
            return static_cast<iterator>(first.node_);
        }

        size_type erase(const key_type& key) {
            size_type count = 0;
            auto it = find(key);
            while (it != end() && comp_(key, KeyOfValue()(*it)) == false && comp_(KeyOfValue()(*it), key) == false) {
                it = erase(it);
                ++count;
            }
            return count;
        }

        void clear() { erase(cbegin(), cend()); }

        /***************************************************
         *                                                 *
         *                 swap                            *
         *                                                 *
         ***************************************************/

        void swap(rb_tree& other) {
            using std::swap;
            swap(header_,     other.header_);
            swap(count_node_, other.count_node_);
            swap(comp_,       other.comp_);
        }

    private:

        using rb_tree_node_allocator = typename Allocator::template rebind<rb_tree_node<value_type>>::other;

        rb_tree_node_allocator alloc;

        rb_tree_node<value_type>* header_;
        size_type                 count_node_;
        Compare                   comp_;

        /****************************************************
         *                                                  *
         *             allocate_node                        *
         *                                                  *
         *             deallocate_node                      *
         *                                                  *
         *             create_node                          *
         *                                                  *
         *             destroy_node                         *
         *                                                  *
         ****************************************************/

        rb_tree_node<value_type>* allocate_node()         { return alloc.allocate(1);      }

        void deallocate_node(rb_tree_node<value_type>* p) {        alloc.deallocate(p, 1); }

        rb_tree_node<value_type>* create_node(const value_type& value) {
            auto new_node = allocate_node();
            tinySTL::construct(&new_node->data_, value);
            return new_node;
        }

        rb_tree_node<value_type>* create_node(value_type&& value) {
            auto new_node = allocate_node();
            tinySTL::construct(&new_node->data_, tinySTL::move(value));
            return new_node;
        }

        void destroy_node(rb_tree_node<value_type>* p) {
            tinySTL::destory(&p->data_);
            deallocate_node(p);
        }

        /*************************************************
         *                                               *
         *                    key                        *
         *                                               *
         *************************************************/

        static const key_type key(rb_tree_node<value_type>* x) { return KeyOfValue()(x->data_); }

        /**************************************************
         *                                                *
         *              insert_aux                        *
         *                                                *
         **************************************************/

        template <typename Y>
        iterator insert_aux(rb_tree_node<value_type>* child, rb_tree_node<value_type>* parent, Y&& value) {
            auto new_node = create_node(std::forward<Y>(value));
            new_node->parent_ = parent;
            new_node->left_   = new_node->right_ = nullptr;
            ++count_node_;

            if (parent == header_)
                header_->parent_ = header_->left_ = header_->right_ = new_node;
            else if (comp_(KeyOfValue()(value), key(parent))) {
                parent->left_ = new_node;
                if (parent == header_->left_)
                    header_->left_ = new_node;
            } else {
                parent->right_ = new_node;
                if (parent == header_->right_)
                    header_->right_ = new_node;
            }
            rb_tree_rebalance(new_node, header_->parent_);
            return static_cast<iterator>(new_node);
        }

        /**************************************************
         *                                                *
         *              insert_equal_aux                  *
         *                                                *
         **************************************************/

        template <typename Y>
        iterator insert_equal_aux(Y&& value) {
            auto parent = header_;
            auto child  = header_->parent_;
            while (child != nullptr) {
                parent = child;
                child  = comp_(KeyOfValue()(value), key(parent)) ? parent->left_ : parent->right_;
            }
            return insert_aux(child, parent, std::forward<Y>(value));
        }

        /**************************************************
         *                                                *
         *              insert_unique_aux                 *
         *                                                *
         **************************************************/

        template <typename Y>
        tinySTL::pair<iterator, bool> insert_unique_aux(Y&& value) {
            auto parent      = header_;
            auto child       = header_->parent_;
            bool comp_result = true;
            while (child != nullptr) {
                parent = child;
                comp_result = comp_(KeyOfValue()(value), key(parent));
                child = comp_result ? parent->left_ : parent->right_;
            }
            auto it = static_cast<iterator>(parent);
            if (comp_result) {
                if (it == begin())
                    return {insert_aux(child, parent, std::forward<Y>(value)), true};
                --it;
            }
            if (comp_(key(it.node_), KeyOfValue()(value)))
                return {insert_aux(child, parent, std::forward<Y>(value)), true};
            return {it, false};
        }

        /*************************************************
         *                                               *
         *              delete_rb_tree                   *
         *                                               *
         *************************************************/

        void delete_rb_tree() {
            if (header_ == nullptr)
                return ;
            clear();
            deallocate_node(header_);
            header_ = nullptr;
        }

        /**************************************************
         *                                                *
         *             move_from                          *
         *                                                *
         **************************************************/

        void move_from(rb_tree& other) {
            delete_rb_tree();
            header_     = other.header_;
            count_node_ = other.count_node_;
            comp_       = other.comp_;
            other.header_ = nullptr;
        }

        /**************************************************
         *                                                *
         *             copy_from                          *
         *                                                *
         **************************************************/

        rb_tree_node<value_type>* copy(const rb_tree_node<value_type>* node) {
            if (node == nullptr)
                return nullptr;
            auto new_node    = create_node(node->data_);
            new_node->color_ = node->color_;
            new_node->left_  = copy(node->left_);
            new_node->right_ = copy(node->right_);
            if (new_node->left_  != nullptr)
                new_node->left_->parent_  = new_node;
            if (new_node->right_ != nullptr)
                new_node->right_->parent_ = new_node;
            return new_node;
        }

        void copy_from(const rb_tree& other) {
            delete_rb_tree();

            count_node_ = other.count_node_;
            comp_       = other.comp_;

            header_          = allocate_node();
            header_->color_  = rb_tree_red;
            header_->parent_ = copy(other.header_->parent_);
            if (header_->parent_ == nullptr)
                header_->left_ = header_->right_ = header_;
            else {
                header_->parent_->parent_ = header_;
                header_->left_ = header_->right_ = header_->parent_;
                while (header_->left_->left_ != nullptr)
                    header_->left_ = header_->left_->left_;
                while (header_->right_->right_ != nullptr)
                    header_->right_ = header_->right_->right_;
            }
        }

    };
}

#endif //TINYSTL_RB_TREE_H
