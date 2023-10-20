#include <cstdlib>
#include "../include/namecheap-cli.hpp"

void print_result(ignacionr::namecheap::response::DomainCheckResult const &dom) {
    std::cout << "available: " << dom.Available 
    << " premium: " << dom.IsPremiumName
    << " premium_registration_price: " << dom.PremiumRegistrationPrice
    << std::endl;
}

int main() {
    ignacionr::namecheap::cli cli(
        std::getenv("NAMECHEAP_USERNAME"), 
        std::getenv("NAMECHEAP_APIKEY"),
        std::getenv("NAMECHEAP_CLIENTIP"));
    std::string proxy_spec = "socks5://localhost:1080";
    cli.set_proxy(proxy_spec);

    auto res = cli.GetDomains();
    std::cout << "domains: " << res.domains().size() << std::endl;
    for (auto const &d: res.domains()) {
        std::cout << d << std::endl;

    }

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
    return 0;
}
