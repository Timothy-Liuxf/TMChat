#ifndef TMCHAT_I_DATABASE_HPP__
#define TMCHAT_I_DATABASE_HPP__

#include <cstdint>
#include <cstddef>
#include <exception>
#include <prep/include/prep.h>

#include "../../common/tmchat.h"

TMCHAT_NAMESPACE_BEGIN

class database_full : ::std::exception
{
public:
    PREP_NODISCARD
    virtual const char*
    what() const noexcept override
    {
        return "The database is full!";
    }
};

template <typename Data, typename Id = default_id_type>
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

    virtual
    ~i_database() = default;
};

TMCHAT_NAMESPACE_END

#endif // #ifndef TMCHAT_I_DATABASE_HPP__
