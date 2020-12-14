template <class T>
inline std::shared_ptr<T> create() {
    std::shared_ptr<T> ret = std::make_shared<T>();
    return ret;
}
