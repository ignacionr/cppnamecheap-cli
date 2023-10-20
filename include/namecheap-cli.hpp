#pragma once
#include <future>
#include <string>
#include <string_view>
#include <iostream>
#include <cpr/cpr.h>
#include <tinyxml2.h>
// #include "DomainCreateParams.hpp"
#include "resp/getdomains.hpp"
// #include "CreateDomainResponse.hpp"
#include "resp/dnshost.hpp"
#include "resp/check.hpp"

namespace ignacionr::namecheap
{
    class cli
    {
    public:
        cli(const std::string &apiUser, const std::string &apiKey, const std::string &clientIp)
            : apiUser(apiUser), apiKey(apiKey), clientIp(clientIp)
        {
            // Base URL for Namecheap API
            baseUrl = "https://api.namecheap.com/xml.response";
        }

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

            cpr::Response r = cpr::Get(cpr::Url{baseUrl}, params, proxies_);
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

    private:
        std::string baseUrl;
        std::string apiUser;
        std::string apiKey;
        std::string clientIp;
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