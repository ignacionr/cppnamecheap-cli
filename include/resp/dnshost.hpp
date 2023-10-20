#pragma once
#include <vector>
#include <tinyxml2.h>
#include "base.hpp"

namespace ignacionr::namecheap::response
{

    struct HostRecord
    {
        HostRecord() = default;
        HostRecord(tinyxml2::XMLElement const &el)
        {
            HostID = el.UnsignedAttribute("HostID");
            Name = el.Attribute("Name");
            Type = el.Attribute("Type");
            Address = el.Attribute("Address");
            MXPref = el.UnsignedAttribute("MXPref");
            TTL = el.UnsignedAttribute("TTL");
        }

        unsigned int HostID{};  //	UniqueID of the host records
        std::string Name;       //	The domain or subdomain for which host record is set
        std::string Type;       //	The type of host record that is set
        std::string Address;    //	The value that is set for the host record(IP address for A record, URL for URL redirects, etc.)
        unsigned int MXPref{};  //	MXPreference number
        unsigned int TTL{3600}; //	TTL value for the host record
    };

    class GetDnsHostsResponse : public ApiResponse
    {
    public:
        auto const &records() const { return records_; }
        auto &records() { return records_; }

    protected:
        void load_from(VisitorFan &fan) override
        {
            fan.element_sink() = [this](auto const &el)
            {
                if (0 == std::strcmp(el.Name(), "host"))
                {
                    records_.push_back(el);
                }
                return true;
            };
        }

    private:
        std::vector<HostRecord> records_;
    };

    class SetDnsHostsResponse : public ApiResponse
    {
    public:
    protected:
        void load_from(VisitorFan &fan) override
        {
            fan.element_sink() = [this](tinyxml2::XMLElement const &el)
            {
                //<DomainDNSSetHostsResult Domain="domain51.com" IsSuccess="true" / >
                if (0 == std::strcmp(el.Name(), "DomainDNSSetHostsResult"))
                {
                    domain_ = el.Attribute("Domain");
                    is_success_ = el.BoolAttribute("IsSuccess");
                }
                return true;
            };
        }

    private:
        std::string domain_;
        bool is_success_;
    };
}