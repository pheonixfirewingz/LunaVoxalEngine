#ifndef RITERATOR_H
#define RITERATOR_H
namespace LunaVoxalEngine::Utils
{
template<typename Iterator> class [[nodiscard]] ReverseIterator final
{
  public:
    constexpr ReverseIterator() = default;

    explicit constexpr ReverseIterator(Iterator iter)
        : current(iter)
    {
    }

    constexpr auto operator*() const
    {
        Iterator temp = current;
        return *--temp;
    }

    constexpr auto operator->() const
    {
        Iterator temp = current;
        --temp;
        return temp.operator->();
    }

    constexpr ReverseIterator &operator++()
    {
        --current;
        return *this;
    }

    constexpr ReverseIterator operator++(int)
    {
        ReverseIterator temp = *this;
        --current;
        return temp;
    }

    constexpr ReverseIterator &operator--()
    {
        ++current;
        return *this;
    }

    constexpr ReverseIterator operator--(int)
    {
        ReverseIterator temp = *this;
        ++current;
        return temp;
    }

    [[nodiscard]] constexpr bool operator==(const ReverseIterator &other) const
    {
        return current == other.current;
    }

    [[nodiscard]] constexpr bool operator!=(const ReverseIterator &other) const
    {
        return !(*this == other);
    }

  private:
    Iterator current;
};

} // namespace LunaVoxalEngine::Utils
#endif
