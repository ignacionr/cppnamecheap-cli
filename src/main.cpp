#include <cstdlib>
#include <string>
#include "../include/namecheap-cli.hpp"

using namespace std::string_literals;

void print_result(ignacionr::namecheap::response::DomainCheckResult const &dom)
{
    std::cout << "available: " << dom.Available
              << " premium: " << dom.IsPremiumName
              << " premium_registration_price: " << dom.PremiumRegistrationPrice
              << std::endl;
}

int main()
{
    ignacionr::namecheap::cli cli(
        std::getenv("NAMECHEAP_USERNAME"),
        std::getenv("NAMECHEAP_APIKEY"),
        std::getenv("NAMECHEAP_CLIENTIP"));
    std::string proxy_spec = "socks5://localhost:1080";
    cli.set_proxy(proxy_spec);

    try
    {

        // auto res = cli.GetPricing();
        // std::cout << res.save() << std::endl;

        // auto res = cli.get_domains();
        // std::cout << "domains: " << res.domains().size() << std::endl;
        // for (auto const &d: res.domains()) {
        //     std::cout << d << std::endl;
        // }

        auto res = cli.get_domains();
        std::cout << "domains: " << res.domains().size() << std::endl;
        for (auto const &d : res.domains())
        {
            std::cout << d.sld() << " on " << d.tld() << std::endl;
            auto res2 = cli.get_hosts(d.sld(), d.tld());
            for (auto &d : res2.results())
            {
                std::cout << d.Domain << ": " << d.hosts_.size() << std::endl;
                for(auto const &h: d.hosts_) {
                    std::cout << h.Type << ": " << h.Name << " -> " << h.Address << std::endl;
                }
            }
        }

        // auto res = cli.get_hosts("webdomina", "com");
        // for (auto &d: res.results()) {
        //     std::cout << d.Domain << ": " << d.hosts_.size() << std::endl;
        //     auto new_hosts = d.hosts_;
        //     bool needs_changing = false;
        //     if (new_hosts.empty()) {
        //         needs_changing = true;
        //         ignacionr::namecheap::Host h;
        //         h.Type = "A";
        //         h.Name = "run";
        //         h.Address = "139.59.85.185";
        //         new_hosts.push_back(h);
        //         h = {};
        //         h.Type = "CNAME";
        //         h.Name = "@";
        //         h.Address = "run.webdomina.com.";
        //         new_hosts.push_back(h);
        //         h = {};
        //         h.Type = "CNAME";
        //         h.Name = "www";
        //         h.Address = "run.webdomina.com.";
        //         new_hosts.push_back(h);
        //     }
        //     else for (auto &h: new_hosts) {
        //         std::cout << h.Type << " " << h.Name << " " << h.Address << "\n";
        //         if (h.Name == "run") {
        //             if (h.Address != "139.59.85.185") {
        //                 h.Address = "139.59.85.185";
        //                 needs_changing = true;
        //             }
        //         }
        //     }
        //     if (needs_changing) {
        //         auto res2 = cli.set_hosts("webdomina", "com", new_hosts);
        //         std::cout << "Modified records " << res2.success << std::endl;
        //         if (!res2.success) {
        //             for(auto const&err: res2.errors) {
        //                 std::cout << err << std::endl;
        //             }
        //         }
        //     }
        // }

        // auto dom3 = cli.check_domains("yetanotherone.com,thismaywork.net,liability.coffee,someonecares.online");
        // std::cout << "yetanotherone.com\n";

        // for(auto const&r: dom3.results()) {
        //     std::cout << r << std::endl;
        // }

        // auto dom3 = cli.CheckDomain("yetanotherone.com");
        // std::cout << "yetanotherone.com\n";
        // print_result(dom3.results()[0]);
        // std::cout << std::flush;
        // auto dom1 = cli.CheckDomainAsync("microsoft.com");
        // auto dom2 = cli.CheckDomainAsync("thisshouldworkyes.com");

        // std::cout << "microsoft.com\n";
        // print_result(dom1.get().results()[0]);
        // std::cout << "thisshouldworkyes.com\n";
        // print_result(dom2.get().results()[0]);
    }
    catch (std::exception &ex)
    {
        std::cerr << "Unable to complete the job: " << ex.what() << std::endl;
        return -1;
    }
    return 0;
}
