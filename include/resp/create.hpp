#pragma once
#include <string>
#include <vector>
#include <tinyxml2.h>
#include "base.hpp"

namespace ignacionr::namecheap::response
{
    struct create : public ApiResponse
    {
    public:
        struct DomainCreateResult
        {
            // <DomainCreateResult Domain="aa.us.com" Registered="true" ChargedAmount="200.8700" DomainID="103877" OrderID="22158" TransactionID="51284" WhoisguardEnable="false" NonRealTimeDomain="false"/>
            DomainCreateResult(tinyxml2::XMLElement const &el)
            {
                Domain = el.Attribute("Domain");
                Registered = el.BoolAttribute("Registered");
                ChargedAmount = el.DoubleAttribute("ChargedAmount");
                DomainID = el.UnsignedAttribute("DomainID");
                OrderID = el.UnsignedAttribute("OrderID");
                TransactionID = el.UnsignedAttribute("TransactionID");
                WhoisguardEnable = el.BoolAttribute("WhoisguardEnable");
                NonRealTimeDomain = el.BoolAttribute("NonRealTimeDomain");
            }
            std::string Domain;
            bool Registered;
            double ChargedAmount;
            uint DomainID;
            uint OrderID;
            uint TransactionID;
            bool WhoisguardEnable;
            bool NonRealTimeDomain;
        };

        const auto &results() const { return results_; }

    protected:
        void load_from(VisitorFan &fan) override
        {
            fan.element_sink() = [this](tinyxml2::XMLElement const &el)
            {
                if (0 == std::strcmp(el.Name(), "DomainCreateResult"))
                {
                    results_.push_back(DomainCreateResult{el});
                }
                return true;
            };
        }

    private:
        std::vector<DomainCreateResult> results_;
    };
}
