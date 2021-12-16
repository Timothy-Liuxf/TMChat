#ifndef TMCHAT_I_DATABASE_HPP__
#define TMCHAT_I_DATABASE_HPP__

#include <cstdint>
#include <cstddef>
#include <prep/include/prep.h>

#include "tmchat.h"

TMCHAT_NAMESPACE_BEGIN

template <typename Data, typename Id = ::std::uint64_t>
class i_database
{
public:
    using data_type = Data;
    using id_type = Id;

    PREP_NODISCARD
    virtual ::std::size_t
    size() const = 0;

    PREP_NODISCARD
    virtual id_type
    add_data(const data_type& data) = 0;

    PREP_NODISCARD
    virtual bool
    id_exists(const id_type& id) const = 0;

    PREP_NODISCARD
    virtual bool
    remove_data(const id_type& id) = 0;

    PREP_NODISCARD
    virtual bool
    get_data(const id_type& id, data_type& out_data) const = 0;
};

TMCHAT_NAMESPACE_END

#endif // #ifndef TMCHAT_I_DATABASE_HPP__
