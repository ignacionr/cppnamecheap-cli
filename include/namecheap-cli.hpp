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
#include "resp/create.hpp"
#include "registration_params.hpp"
#include "resp/gethosts.hpp"

namespace ignacionr::namecheap
{
    class cli
    {
    public:
        cli(const std::string &apiUser, const std::string &apiKey, const std::string &clientIp)
            : apiUser(apiUser), apiKey(apiKey), clientIp(clientIp)
        {
            ;
        }

        void set_proxy(std::string const &proxy_spec)
        {
            proxies_ = cpr::Proxies{{"http", proxy_spec}, {"https", proxy_spec}};
        }

        using operation_t = std::function<cpr::Response(const cpr::Url &, const cpr::Parameters &, const cpr::Proxies &)>;

        template <typename T>
        T Op(operation_t op, std::string_view command, std::vector<std::pair<std::string, std::string>> const &additional_params = {})
        {
            cpr::Parameters params;
            AddCommonParameters(params, command);
            for (auto const &p : additional_params)
            {
                params.Add({p.first, p.second});
            }

            cpr::Response r = op(baseUrl, params, proxies_);
            if (r.error.code != cpr::ErrorCode::OK)
            {
                throw std::runtime_error(r.error.message);
            }
            T result;
            result.load(r.text);

            return result;
        }

        template <typename T>
        T Get(std::string_view command, std::vector<std::pair<std::string, std::string>> const &additional_params = {})
        {
            return Op<T>([](auto const &a, auto const &b, auto const &c)
                         { return cpr::Get(a, b, c); },
                         command, additional_params);
        }

        template <typename T>
        T Post(std::string_view command, std::vector<std::pair<std::string, std::string>> const &additional_params = {})
        {
            return Op<T>([](auto const &a, auto const &b, auto const &c)
                         { return cpr::Post(a, b, c); },
                         command, additional_params);
        }

        response::domains_check CheckDomain(const std::string &domainName)
        {
            return Get<response::domains_check>("namecheap.domains.check", {{"DomainList", domainName}});
        }

        response::getdomains GetDomains()
        {
            return Get<response::getdomains>("namecheap.domains.getlist");
        }

        response::getpricing GetPricing()
        {
            return Get<response::getpricing>("namecheap.users.getPricing",
                                             {{"ProductType", "DOMAIN"},
                                              {"ProductCategory", "DOMAINS"},
                                              {"ActionName", "REGISTER"}

                                             });
        }

        response::create CreateDomain(std::string const &DomainName,
                                      int Years,
                                      const RegistrationParams &params)
        {
            std::vector<std::pair<std::string, std::string>> parameters{
                {"DomainName", DomainName},
                {"Years", std::to_string(Years)}};
            auto strategy = [&parameters](const std::string &name, const std::string &value)
            {
                parameters.push_back({name, value});
            };
            params.save_to(strategy);

            return Post<response::create>("namecheap.domains.create", parameters);
        }

        response::gethost GetHost(std::string const &sld, std::string const &tld)
        {
            return Get<response::gethost>("namecheap.domains.dns.getHosts",
                                          {{"SLD", sld}, {"TLD", tld}});
        }

    private:
        std::string apiUser;
        std::string apiKey;
        std::string clientIp;
        const cpr::Url baseUrl{"https://api.namecheap.com/xml.response"};
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
