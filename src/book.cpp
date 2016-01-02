#include "book.hpp"

// This is important. The top of the BUY book should be the highest price. Meaining, the highest bidder
// should match the lowest seller. The spread is what's inbetween.
template<> Order<Buy> Book<Buy>::top_of_book() { return m_orders.rbegin()->second.front(); };
template<> Order<Sell> Book<Sell>::top_of_book() { return m_orders.begin()->second.front(); };
