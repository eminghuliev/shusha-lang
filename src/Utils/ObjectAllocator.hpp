#ifndef OBJECT_ALLOCATOR_HPP
#define OBJECT_ALLOCATOR_HPP
template <class T>
inline std::shared_ptr<T> create() {
    std::shared_ptr<T> ret = std::make_shared<T>();
    return ret;
}
#endif
