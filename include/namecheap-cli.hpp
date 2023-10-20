#pragma once

#include <future>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

#include <cpr/cpr.h>

// #include "DomainCreateParams.hpp"
#include "resp/getdomains.hpp"
// #include "CreateDomainResponse.hpp"
#include "resp/dnshost.hpp"
#include "resp/check.hpp"
#include "resp/getpricing.hpp"

namespace ignacionr::namecheap
{
    class cli
    {
    public:
        cli(const std::string &apiUser, const std::string &apiKey, const std::string &clientIp)
            : apiUser(apiUser), apiKey(apiKey), clientIp(clientIp)
        { ; }

        void set_proxy(std::string const &proxy_spec)
        {
            proxies_ = cpr::Proxies{{"http", proxy_spec}, {"https", proxy_spec}};
        }

        template <typename T>
        T Get(std::string_view command, std::vector<std::pair<std::string,std::string>> const &additional_params = {})
        {
            cpr::Parameters params;
            AddCommonParameters(params, command);
            for(auto const&p: additional_params) {
                params.Add({p.first, p.second});
            }

            cpr::Response r = cpr::Get(baseUrl, params, proxies_);
            if (r.error.code != cpr::ErrorCode::OK) {
                throw std::runtime_error(r.error.message);
            }
            T result;
            result.load(r.text);

            return result;
        }

        response::domains_check CheckDomain(const std::string &domainName)
        {
            return Get<response::domains_check>("namecheap.domains.check", {{"DomainList", domainName}});
        }

        std::future<response::domains_check> CheckDomainAsync(const std::string &domainName)
        {
            return std::async([this, domainName]
                              { return CheckDomain(domainName); });
        }

        response::getdomains GetDomains()
        {
            return Get<response::getdomains>("namecheap.domains.getlist");
        }

        std::future<response::getdomains> GetDomainsAsync()
        {
            return std::async([this]
                              { return GetDomains(); });
        }

        response::getpricing GetPricing() {
            return Get<response::getpricing>("namecheap.users.getPricing",
            {
                {"ProductType", "DOMAIN"},
                {"ProductCategory", "DOMAINS"},
                {"ActionName", "REGISTER"}

            });
        }

    private:
        std::string apiUser;
        std::string apiKey;
        std::string clientIp;
        const cpr::Url baseUrl {"https://api.namecheap.com/xml.response"};
        cpr::Proxies proxies_;

        void AddCommonParameters(cpr::Parameters &params, std::string_view command_name)
        {
            params.Add({{"ApiUser", apiUser},
                        {"ApiKey", apiKey},
                        {"UserName", apiUser},
                        {"ClientIp", clientIp},
                        {"Command", std::string(command_name)}});
        }
    };
}
