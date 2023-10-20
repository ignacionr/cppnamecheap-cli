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
    class sethost : public ApiResponse {
    public:

    struct DomainDNSSetHostsResult{
        std::string Domain;
        bool IsSuccess;
    };
    protected:
        virtual void load_from(VisitorFan &fan) 
        {
            fan.element_sink() = [this, &fan](tinyxml2::XMLElement const &el){
                if (0 == std::strcmp(el.Name(), "DomainDNSSetHostsResult")) {
                    results_.push_back({
                        el.Attribute("Domain"),
                        el.BoolAttribute("IsSuccess")
                    });
                }
                return true;
            };
        }
    private:
        std::vector<DomainDNSSetHostsResult> results_;
    };
}
