#pragma once
#include <string>
#include <tinyxml2.h>
#include "base.hpp"

namespace ignacionr::namecheap::response
{

    struct DomainCheckResult
    {
        DomainCheckResult(const tinyxml2::XMLElement &el)
        {
            Domain = el.Attribute("Domain");
            Available = el.BoolAttribute("Available");
            ErrorNo = el.UnsignedAttribute("ErrorNo");
            Description = el.Attribute("Description");
            IsPremiumName = el.BoolAttribute("IsPremiumName");
            PremiumRegistrationPrice = el.DoubleAttribute("PremiumRegistrationPrice");
            PremiumRenewalPrice = el.DoubleAttribute("PremiumRenewalPrice");
            PremiumRestorePrice = el.DoubleAttribute("PremiumRestorePrice");
            PremiumTransferPrice = el.DoubleAttribute("PremiumTransferPrice");
            IcannFee = el.DoubleAttribute("IcannFee");
            EapFee = el.DoubleAttribute("EapFee");
        }

        // Friend function for operator<<
        friend std::ostream &operator<<(std::ostream &os, const DomainCheckResult &result)
        {
            os << "Domain: " << result.Domain << '\n'
               << "Available: " << std::boolalpha << result.Available << '\n'
               << "ErrorNo: " << result.ErrorNo << '\n'
               << "Description: " << result.Description << '\n'
               << "IsPremiumName: " << std::boolalpha << result.IsPremiumName << '\n'
               << "PremiumRegistrationPrice: " << result.PremiumRegistrationPrice << '\n'
               << "PremiumRenewalPrice: " << result.PremiumRenewalPrice << '\n'
               << "PremiumRestorePrice: " << result.PremiumRestorePrice << '\n'
               << "PremiumTransferPrice: " << result.PremiumTransferPrice << '\n'
               << "IcannFee: " << result.IcannFee << '\n'
               << "EapFee: " << result.EapFee;
            return os;
        }
        std::string Domain;
        bool Available;
        uint ErrorNo;
        std::string Description;
        bool IsPremiumName;
        double PremiumRegistrationPrice;
        double PremiumRenewalPrice;
        double PremiumRestorePrice;
        double PremiumTransferPrice;
        double IcannFee;
        double EapFee;
    };

    class domains_check : public ApiResponse
    {
    public:
        std::vector<DomainCheckResult> const &results() const { return results_; }

    protected:
        void load_from(VisitorFan &fan) override
        {
            fan.element_sink() = [this](tinyxml2::XMLElement const &el)
            {
                if (0 == std::strcmp(el.Name(), "DomainCheckResult"))
                {
                    results_.push_back(DomainCheckResult{el});
                }
                return true;
            };
        }

    private:
        std::vector<DomainCheckResult> results_;
    };
}

// <ApiResponse Status="OK" xmlns="http://api.namecheap.com/xml.response">
//   <Errors />
//   <Warnings />
//   <RequestedCommand>namecheap.domains.check</RequestedCommand>
//   <CommandResponse Type="namecheap.domains.check">
//     <DomainCheckResult Domain="thisshouldworkyes.com" Available="true" ErrorNo="0" Description="" IsPremiumName="false" PremiumRegistrationPrice="0" PremiumRenewalPrice="0" PremiumRestorePrice="0" PremiumTransferPrice="0" IcannFee="0" EapFee="0.0" />
//   </CommandResponse>
//   <Server>PHX01APIEXT01</Server>
//   <GMTTimeDifference>--4:00</GMTTimeDifference>
//   <ExecutionTime>0.288</ExecutionTime>
// </ApiResponse>
