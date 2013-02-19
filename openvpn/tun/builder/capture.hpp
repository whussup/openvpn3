//
//  capture.hpp
//  OpenVPN
//
//  Copyright (c) 2012 OpenVPN Technologies, Inc. All rights reserved.
//

// An artificial TunBuilder object, used to log the tun builder settings,
// but doesn't actually configure anything.

#ifndef OPENVPN_TUN_BUILDER_CAPTURE_H
#define OPENVPN_TUN_BUILDER_CAPTURE_H

#include <string>
#include <sstream>
#include <vector>

#include <openvpn/common/rc.hpp>
#include <openvpn/tun/builder/base.hpp>
#include <openvpn/client/rgopt.hpp>

namespace openvpn {
  class TunBuilderCapture : public TunBuilderBase, public RC<thread_unsafe_refcount>
  {
  public:
    typedef boost::intrusive_ptr<TunBuilderCapture> Ptr;

    // builder data classes

    struct RemoteAddress {
      RemoteAddress() : ipv6(false) {}
      std::string address;
      bool ipv6;

      std::string to_string() const
      {
	std::string ret = address;
	if (ipv6)
	  ret += " [IPv6]";
	return ret;
      }
    };

    struct RerouteGW {
      RerouteGW() : ipv4(false), ipv6(false), flags(0) {}
      bool ipv4;
      bool ipv6;
      unsigned int flags;

      std::string to_string() const
      {
	std::ostringstream os;
	const RedirectGatewayFlags rgf(flags);
	os << "IPv4=" << ipv4 << "  IPv6=" << ipv6 << " flags=" << rgf.to_string();
	return os.str();
      }
    };

    struct Route {
      Route() : prefix_length(0), ipv6(false) {}
      std::string address;
      int prefix_length;
      bool ipv6;

      std::string to_string() const
      {
	std::ostringstream os;
	os << address << '/' << prefix_length;
	if (ipv6)
	  os << " [IPv6]";
	return os.str();
      }
    };

    struct DNSServer {
      DNSServer() : ipv6(false) {}
      std::string address;
      bool ipv6;

      std::string to_string() const
      {
	std::string ret = address;
	if (ipv6)
	  ret += " [IPv6]";
	return ret;
      }
    };

    struct SearchDomain {
      std::string domain;

      std::string to_string() const
      {
	return domain;
      }
    };

    TunBuilderCapture() : mtu(1500) {}

    virtual bool tun_builder_set_remote_address(const std::string& address, bool ipv6)
    {
      remote_address.address = address;
      remote_address.ipv6 = ipv6;
      return true;
    }

    virtual bool tun_builder_add_address(const std::string& address, int prefix_length, bool ipv6)
    {
      Route r;
      r.address = address;
      r.prefix_length = prefix_length;
      r.ipv6 = ipv6;
      tunnel_addresses.push_back(r);
      return true;
    }

    virtual bool tun_builder_reroute_gw(const std::string& server_address, bool server_address_ipv6, bool ipv4, bool ipv6, unsigned int flags)
    {
      reroute_gw.ipv4 = ipv4;
      reroute_gw.ipv6 = ipv6;
      reroute_gw.flags = flags;
      return true;
    }

    virtual bool tun_builder_add_route(const std::string& address, int prefix_length, bool ipv6)
    {
      Route r;
      r.address = address;
      r.prefix_length = prefix_length;
      r.ipv6 = ipv6;
      add_routes.push_back(r);
      return true;
    }

    virtual bool tun_builder_exclude_route(const std::string& address, int prefix_length, bool ipv6)
    {
      Route r;
      r.address = address;
      r.prefix_length = prefix_length;
      r.ipv6 = ipv6;
      exclude_routes.push_back(r);
      return true;
    }

    virtual bool tun_builder_add_dns_server(const std::string& address, bool ipv6)
    {
      DNSServer dns;
      dns.address = address;
      dns.ipv6 = ipv6;
      dns_servers.push_back(dns);
      return true;
    }

    virtual bool tun_builder_add_search_domain(const std::string& domain)
    {
      SearchDomain dom;
      dom.domain = domain;
      search_domains.push_back(dom);
      return true;
    }

    virtual bool tun_builder_set_mtu(int mtu)
    {
      this->mtu =  mtu;
      return true;
    }

    virtual bool tun_builder_set_session_name(const std::string& name)
    {
      session_name = name;
      return true;
    }

    std::string to_string() const
    {
      std::ostringstream os;
      os << "Session Name: " << session_name << std::endl;
      os << "MTU: " << mtu << std::endl;
      os << "Remote Address: " << remote_address.to_string() << std::endl;
      render_route_list(os, "Tunnel Addresses", tunnel_addresses);
      os << "Reroute Gateway: " << reroute_gw.to_string() << std::endl;
      render_route_list(os, "Add Routes", add_routes);
      render_route_list(os, "Exclude Routes", exclude_routes);
      {
	os << "DNS Servers:" << std::endl;
	for (std::vector<DNSServer>::const_iterator i = dns_servers.begin(); i != dns_servers.end(); ++i)
	  os << "  " << i->to_string() << std::endl;
      }
      {
	os << "Search Domains:" << std::endl;
	for (std::vector<SearchDomain>::const_iterator i = search_domains.begin(); i != search_domains.end(); ++i)
	  os << "  " << i->to_string() << std::endl;
      }
      return os.str();
    }

    // builder data
    std::string session_name;
    int mtu;
    RemoteAddress remote_address;          // real address of server
    std::vector<Route> tunnel_addresses;   // local tunnel addresses
    RerouteGW reroute_gw;                  // redirect-gateway info for ipv4
    std::vector<Route> add_routes;         // routes that should be added to tunnel
    std::vector<Route> exclude_routes;     // routes that should be excluded from tunnel
    std::vector<DNSServer> dns_servers;    // VPN DNS servers
    std::vector<SearchDomain> search_domains;  // domain suffixes whose DNS requests should be tunnel-routed

  private:
    void render_route_list(std::ostream& os, const char *title, const std::vector<Route>& list) const
    {
      os << title << ':' << std::endl;
      for (std::vector<Route>::const_iterator i = list.begin(); i != list.end(); ++i)
	os << "  " << i->to_string() << std::endl;
    }
  };
}

#endif