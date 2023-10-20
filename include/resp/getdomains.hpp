#pragma once
#include <functional>
#include <string>
#include <vector>
#include "base.hpp"
#include "conversions.hpp"

namespace ignacionr::namecheap::response
{
    struct Domain
    {
        //      <Domain ID="70727186" Name="webdomina.com" User="eliciam" Created="09/03/2023" Expires="09/03/2025" IsExpired="false" IsLocked="false" AutoRenew="false" WhoisGuard="ENABLED" IsPremium="false" IsOurDNS="true" />
        unsigned long ID;
        std::string Name;
        std::string User;
        std::chrono::system_clock::time_point Created;
        std::chrono::system_clock::time_point Expires;
        bool IsExpired;
        bool IsLocked;
        bool AutoRenew;
        std::string WhoisGuard;
        bool IsPremium;
        bool IsOurDNS;
    };

    std::ostream &operator<<(std::ostream &os, const std::chrono::system_clock::time_point &tp)
    {
        std::time_t time = std::chrono::system_clock::to_time_t(tp);
        os << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const Domain &domain)
    {
        os << "Domain Info:\n"
           << "  ID: " << domain.ID << '\n'
           << "  Name: " << domain.Name << '\n'
           << "  User: " << domain.User << '\n'
           << "  Created: " << domain.Created << '\n'
           << "  Expires: " << domain.Expires << '\n'
           << "  IsExpired: " << std::boolalpha << domain.IsExpired << '\n'
           << "  IsLocked: " << std::boolalpha << domain.IsLocked << '\n'
           << "  AutoRenew: " << std::boolalpha << domain.AutoRenew << '\n'
           << "  WhoisGuard: " << domain.WhoisGuard << '\n'
           << "  IsPremium: " << std::boolalpha << domain.IsPremium << '\n'
           << "  IsOurDNS: " << std::boolalpha << domain.IsOurDNS;

        return os;
    }

    template <typename ostream_t>
    ostream_t &operator<<(ostream_t &out, Domain const &d)
    {
        out << "ID: " << d.ID << std::endl;
        out << "Name: " << d.Name << std::endl;
        return out;
    }

    Domain to_response(tinyxml2::XMLElement const &el)
    {
        return {
            el.UnsignedAttribute("ID"),
            el.Attribute("Name"),
            el.Attribute("User"),
            convert::USDateToTimePoint(el.Attribute("Created")),
            convert::USDateToTimePoint(el.Attribute("Expires")),
            el.BoolAttribute("IsExpired"),
            el.BoolAttribute("IsLocked"),
            el.BoolAttribute("AutoRenew"),
            el.Attribute("WhoisGuard"),
            el.BoolAttribute("IsPremium"),
            el.BoolAttribute("IsOurDNS")};
    };

    class getdomains : public ApiResponse
    {
    public:
        auto const &domains() const { return domains_; }
        auto total_items() const { return TotalItems; }
        auto current_page() const { return CurrentPage; }
        auto page_size() const { return PageSize; }

    private:
        void load_from(VisitorFan &visitor) override
        {
            visitor.element_sink() = [this, &visitor](tinyxml2::XMLElement const &element) -> bool
            {
                if (0 == std::strcmp("Domain", element.Name()))
                {
                    domains_.push_back(to_response(element));
                }
                else if (0 == std::strcmp("TotalItems", element.Name()))
                {
                    visitor.text_sink() = [this, &visitor](tinyxml2::XMLText const &text) -> bool
                    {
                        TotalItems = std::strtoul(text.Value(), nullptr, 10);
                        visitor.text_sink() = {};
                        return true;
                    };
                }
                else if (0 == std::strcmp("CurrentPage", element.Name()))
                {
                    visitor.text_sink() = [this, &visitor](tinyxml2::XMLText const &text) -> bool
                    {
                        CurrentPage = std::strtoul(text.Value(), nullptr, 10);
                        visitor.text_sink() = {};
                        return true;
                    };
                }
                else if (0 == std::strcmp("PageSize", element.Name()))
                {
                    visitor.text_sink() = [this, &visitor](tinyxml2::XMLText const &text) -> bool
                    {
                        PageSize = std::strtoul(text.Value(), nullptr, 10);
                        visitor.text_sink() = {};
                        return true;
                    };
                }
                return true;
            };
        }

    private:
        std::vector<Domain> domains_;
        unsigned long TotalItems;
        unsigned long CurrentPage;
        unsigned long PageSize;
    };
}

//<?xml version="1.0" encoding="utf-8"?>
//<ApiResponse Status="OK" xmlns="http://api.namecheap.com/xml.response">
//  <Errors />
//  <Warnings />
//  <RequestedCommand>namecheap.domains.getlist</RequestedCommand>
//  <CommandResponse Type="namecheap.domains.getList">
//    <DomainGetListResult>
//      <Domain ID="70727186" Name="webdomina.com" User="eliciam" Created="09/03/2023" Expires="09/03/2025" IsExpired="false" IsLocked="false" AutoRenew="false" WhoisGuard="ENABLED" IsPremium="false" IsOurDNS="true" />
//    </DomainGetListResult>
//    <Paging>
//      <TotalItems>1</TotalItems>
//      <CurrentPage>1</CurrentPage>
//      <PageSize>100</PageSize>
//    </Paging>
//  </CommandResponse>
//  <Server>PHX01APIEXT02</Server>
//  <GMTTimeDifference>--4:00</GMTTimeDifference>
//  <ExecutionTime>0.074</ExecutionTime>
//</ApiResponse>