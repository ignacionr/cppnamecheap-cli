#pragma once
#include <optional>
#include <string>
#include <vector>
#include <tinyxml2.h>
#include "base.hpp"

namespace ignacionr::namecheap::response
{
    class getpricing : public ApiResponse
    {
    public:
        struct Price_t
        {
            // <Price Duration="1" DurationType="YEAR" Price="8.55" RegularPrice="8.55" YourPrice="8.55"
            // CouponPrice="" Currency="USD" />
            Price_t(tinyxml2::XMLElement const &el)
            {
                // tinyxml2::XMLPrinter printer;
                // el.Accept(&printer);
                // std::cerr << printer.CStr() << std::endl;

                Duration = el.UnsignedAttribute("Duration");
                DurationType = el.Attribute("DurationType");
                Price = el.DoubleAttribute("Price");
                RegularPrice = el.DoubleAttribute("RegularPrice");
                YourPrice = el.DoubleAttribute("YourPrice");
                CouponPrice = el.DoubleAttribute("CouponPrice");
                if (0.0 == CouponPrice)
                    CouponPrice = {};
                Currency = el.Attribute("Currency");
            }
            uint Duration;
            std::string DurationType;
            double Price;
            double RegularPrice;
            double YourPrice;
            std::optional<double> CouponPrice;
            std::string Currency;
        };

        struct Product
        {
            std::string Name;
            std::vector<Price_t> Prices;

            Product(tinyxml2::XMLElement const &el)
            {
                Name = el.Attribute("Name");
            }

            bool read(tinyxml2::XMLElement const &el)
            {
                if (0 == std::strcmp(el.Name(), "Price"))
                {
                    Prices.push_back({el});
                }
                return true;
            }
        };

    protected:
        void load_from(VisitorFan &fan) override
        {
            fan.element_sink() = [this, &fan](tinyxml2::XMLElement const &el)
            {
                if (0 == std::strcmp(el.Name(), "Product"))
                {
                    products_.push_back({el});
                    auto &prod{products_.back()};
                    fan.push(&el);
                    fan.element_sink() = [&prod](auto const &el)
                    { 
                        return prod.read(el);
                    };
                }
                return true;
            };
        }

        void save_to(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *root) const override
        {
            auto *commandResponse = doc.NewElement("CommandResponse");
            root->InsertEndChild(commandResponse);

            for (const auto &product : products_)
            {
                auto *productElement = doc.NewElement("Product");
                productElement->SetAttribute("Name", product.Name.c_str());
                commandResponse->InsertEndChild(productElement);

                for (const auto &price : product.Prices)
                {
                    auto *priceElement = doc.NewElement("Price");
                    priceElement->SetAttribute("Duration", price.Duration);
                    priceElement->SetAttribute("DurationType", price.DurationType.c_str());
                    priceElement->SetAttribute("Price", price.Price);
                    priceElement->SetAttribute("RegularPrice", price.RegularPrice);
                    priceElement->SetAttribute("YourPrice", price.YourPrice);

                    if (price.CouponPrice.has_value())
                    {
                        priceElement->SetAttribute("CouponPrice", *price.CouponPrice);
                    }
                    else
                    {
                        priceElement->SetAttribute("CouponPrice", "");
                    }

                    priceElement->SetAttribute("Currency", price.Currency.c_str());
                    productElement->InsertEndChild(priceElement);
                }
            }
        }

    private:
        std::vector<Product> products_;
    };
}
