#pragma once

#include <future>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

#include <cpr/cpr.h>

#include "resp/getdomains.hpp"
#include "resp/dnshost.hpp"
#include "resp/check.hpp"
#include "resp/getpricing.hpp"
#include "resp/create.hpp"
#include "registration_params.hpp"
#include "resp/gethosts.hpp"
#include "resp/sethosts.hpp"

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
                         { return cpr::Get(a, b, c, cpr::Timeout(2000)); },
                         command, additional_params);
        }

        template <typename T>
        T Post(std::string_view command, std::vector<std::pair<std::string, std::string>> const &additional_params = {})
        {
            return Op<T>([](auto const &a, auto const &b, auto const &c)
                         { return cpr::Post(a, b, c); },
                         command, additional_params);
        }

        template <typename T>
        T throw_if_error(T const &result)
        {
            if (!result.success)
            {
                throw std::runtime_error(std::accumulate(result.errors.begin(),
                                                         result.errors.end(),
                                                         std::string{},
                                                         [](std::string r, std::string const &e)
                                                         {
                                                             r += e;
                                                             r += "\n";
                                                             return r;
                                                         }));
            }
            return result;
        }

        response::domains_check
        check_domains(const std::string &domain_names)
        {
            return throw_if_error(Get<response::domains_check>("namecheap.domains.check", {{"DomainList", domain_names}}));
        }

        response::getdomains get_domains()
        {
            return throw_if_error(Get<response::getdomains>("namecheap.domains.getlist"));
        }

        response::getpricing get_pricing()
        {
            return throw_if_error(Get<response::getpricing>("namecheap.users.getPricing",
                                             {{"ProductType", "DOMAIN"},
                                              {"ProductCategory", "DOMAINS"},
                                              {"ActionName", "REGISTER"}

                                             }));
        }

        response::create create_domain(std::string const &DomainName,
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

            return throw_if_error(Post<response::create>("namecheap.domains.create", parameters));
        }

        response::gethost get_hosts(std::string const &sld, std::string const &tld)
        {
            return throw_if_error(Get<response::gethost>("namecheap.domains.dns.getHosts",
                                          {{"SLD", sld}, {"TLD", tld}}));
        }

        template <typename host_list_t>
        response::sethost set_hosts(std::string const &sld, std::string const &tld, host_list_t const &hosts)
        {
            std::vector<std::pair<std::string, std::string>> params{{"SLD", sld}, {"TLD", tld}};
            int count{};
            for (Host const &h : hosts)
            {
                std::string const idx{std::to_string(++count)};
                params.push_back({"Address" + idx, h.Address});
                if (h.HostId)
                    params.push_back({"HostId" + idx, std::to_string(h.HostId)});
                if (h.MXPref.has_value())
                {
                    params.push_back({"MXPref" + idx, std::to_string(h.MXPref.value())});
                }
                params.push_back({"HostName" + idx, h.Name});
                params.push_back({"TTL" + idx, std::to_string(h.TTL)});
                params.push_back({"RecordType" + idx, h.Type});
            }
            return throw_if_error(Get<response::sethost>("namecheap.domains.dns.setHosts", params));
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
