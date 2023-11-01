#pragma once
#include <functional>
#include <string>
#include <optional>
#include <vector>
#include <tinyxml2.h>
#include "base.hpp"
#include "../host.hpp"

namespace ignacionr::namecheap::response
{
    class gethost : public ApiResponse
    {
    public:
        struct DomainDNSGetHostsResult
        {
            std::string Domain;
            bool IsUsingOurDNS;
            std::vector<Host> hosts_;
            bool read_inside(tinyxml2::XMLElement const &el)
            {
                if (0 == std::strcmp(el.Name(), "host"))
                {
                    hosts_.push_back({el});
                }
                return true;
            }
        };
        auto const& results() const { return results_; }
        auto & results() { return results_; }

    protected:
        virtual void load_from(VisitorFan &fan)
        {
            fan.element_sink() = [this, &fan](tinyxml2::XMLElement const &el)
            {
                if (0 == std::strcmp(el.Name(), "DomainDNSGetHostsResult"))
                {
                    results_.push_back({el.Attribute("Domain"),
                                        el.BoolAttribute("IsUsingOurDNS")});
                    auto &r = results_.back();
                    fan.push(&el).element_sink() = [&r](tinyxml2::XMLElement const &el)
                    {
                        return r.read_inside(el);
                    };
                }
                return true;
            };
        }

    private:
        std::vector<DomainDNSGetHostsResult> results_;
    };
}

// <?xml version="1.0" encoding="UTF-8"?>
// <ApiResponse xmlns="http://api.namecheap.com/xml.response" Status="OK">
//   <Errors />
//   <RequestedCommand>namecheap.domains.dns.getHosts</RequestedCommand>
//   <CommandResponse Type="namecheap.domains.dns.getHosts">
//     <DomainDNSGetHostsResult Domain="domain.com" IsUsingOurDNS="true">
//       <host HostId="12" Name="@" Type="A" Address="1.2.3.4" MXPref="10" TTL="1800" />
//       <host HostId="14" Name="www" Type="A" Address="122.23.3.7" MXPref="10" TTL="1800" />
//     </DomainDNSGetHostsResult>
//   </CommandResponse>
//   <Server>SERVER-NAME</Server>
//   <GMTTimeDifference>+5</GMTTimeDifference>
//   <ExecutionTime>32.76</ExecutionTime>
// </ApiResponse>