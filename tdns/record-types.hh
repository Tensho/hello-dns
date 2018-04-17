#pragma once
#include <memory>
#include "dns-storage.hh"
#include "dnsmessages.hh"
#include "comboaddress.hh"

class DNSMessageReader;

/*! 
   @file
   @brief Defines all Resource Record Generators

   Generators know about a record type's contents. 
   They also know how to inject themselves into a DNSMessageWriter, parse themselves from a DNSMessageReader 
   and how to convert themselves into a master file representation.
 */


//! IP address, A record generator
struct AGen : RRGen
{
  AGen(uint32_t ip) : d_ip(ip) {}
  AGen(DNSMessageReader& dmr);

  static std::unique_ptr<RRGen> make(const ComboAddress&);
  static std::unique_ptr<RRGen> make(const std::string& s)
  {
    return make(ComboAddress(s));
  }
  void toMessage(DNSMessageWriter& dpw) override; // to packet/message
  std::string toString() const override; // master zone format
  DNSType getType() const override { return DNSType::A; }

  uint32_t d_ip; // the actual IP
};
//! Generates an AAAA (IPv6 address) record
struct AAAAGen : RRGen
{
  AAAAGen(DNSMessageReader& dmr);
  AAAAGen(unsigned char ip[16])
  {
    memcpy(d_ip, ip, 16);
  }
  static std::unique_ptr<RRGen> make(const ComboAddress&);
  static std::unique_ptr<RRGen> make(const std::string& s)
  {
    return make(ComboAddress(s));
  }
  void toMessage(DNSMessageWriter& dpw) override;
  std::string toString() const override;
  DNSType getType() const override { return DNSType::AAAA; }

  unsigned char d_ip[16];
};

//! Generates a SOA Resource Record
struct SOAGen : RRGen
{
  SOAGen(const DNSName& mname, const DNSName& rname, uint32_t serial, uint32_t minimum=3600, uint32_t refresh=10800, uint32_t retry=3600, uint32_t expire=604800) :
    d_mname(mname), d_rname(rname), d_serial(serial), d_minimum(minimum), d_refresh(refresh), d_retry(retry), d_expire(expire)
  {}

  SOAGen(DNSMessageReader& dmr);
  
  static std::unique_ptr<RRGen> make(const DNSName& mname, const DNSName& rname, uint32_t serial, uint32_t minimum=3600, uint32_t refresh=10800, uint32_t retry=3600, uint32_t expire=604800)
  {
    return std::make_unique<SOAGen>(mname, rname, serial, minimum, refresh, retry, expire);
  }
  
  void toMessage(DNSMessageWriter& dpw) override;
  DNSType getType() const override { return DNSType::SOA; }
  std::string toString() const override;
  template<typename X> void doConv(X& x);
  DNSName d_mname, d_rname;
  uint32_t d_serial, d_minimum, d_refresh, d_retry, d_expire;
};

//! Generates a SRV Resource Record
struct SRVGen : RRGen
{
  SRVGen(uint16_t preference, uint16_t weight, uint16_t port, const DNSName& target) : 
    d_preference(preference), d_weight(weight), d_port(port), d_target(target)
  {}

  SRVGen(DNSMessageReader& dmr);
  void toMessage(DNSMessageWriter& dpw) override;
  DNSType getType() const override { return DNSType::SRV; }
  std::string toString() const override;

  template<typename X> void doConv(X& x);

  uint16_t d_preference, d_weight, d_port;
  DNSName d_target;
};

//! Generates a NAPTR Resource Record
struct NAPTRGen : RRGen
{
  NAPTRGen(uint16_t order, uint16_t pref, const std::string& flags,
           const std::string& services, const std::string& regexp,
           const DNSName& replacement) : 
    d_order(order), d_pref(pref), d_flags(flags), d_services(services), d_regexp(regexp), d_replacement(replacement)
  {}

  NAPTRGen(DNSMessageReader& dmr);
  void toMessage(DNSMessageWriter& dpw) override;
  DNSType getType() const override { return DNSType::NAPTR; }
  std::string toString() const override;
  template<typename X> void doConv(X& x);

  uint16_t d_order, d_pref;
  std::string d_flags, d_services, d_regexp;
  DNSName d_replacement;
};


//! Generates a CNAME Resource Record
struct CNAMEGen : RRGen
{
  CNAMEGen(const DNSName& name) : d_name(name) {}
  CNAMEGen(DNSMessageReader& dmr);
  static std::unique_ptr<RRGen> make(const DNSName& mname)
  {
    return std::make_unique<CNAMEGen>(mname);
  }
  void toMessage(DNSMessageWriter& dpw) override;
  std::string toString() const override;
  DNSType getType() const override { return DNSType::CNAME; }
  
  DNSName d_name;
};

//! Generates a PTR Resource Record
struct PTRGen : RRGen
{
  PTRGen(const DNSName& name) : d_name(name) {}
  PTRGen(DNSMessageReader& dmr);
  static std::unique_ptr<RRGen> make(const DNSName& mname)
  {
    return std::make_unique<PTRGen>(mname);
  }
  void toMessage(DNSMessageWriter& dpw) override;
  std::string toString() const override;
  DNSType getType() const override { return DNSType::PTR; }
  DNSName d_name;
};

//! Generates an NS Resource Record
struct NSGen : RRGen
{
  NSGen(const DNSName& name) : d_name(name) {}
  NSGen(DNSMessageReader& dmr);
  static std::unique_ptr<RRGen> make(const DNSName& mname)
  {
    return std::make_unique<NSGen>(mname);
  }
  void toMessage(DNSMessageWriter& dpw) override;
  std::string toString() const override;
  DNSType getType() const override { return DNSType::NS; }
  DNSName d_name;
};

//! Generates an MX Resource Record
struct MXGen : RRGen
{
  MXGen(uint16_t prio, const DNSName& name) : d_prio(prio), d_name(name) {}
  MXGen(DNSMessageReader& dmr);
  
  static std::unique_ptr<RRGen> make(uint16_t prio, const DNSName& name)
  {
    return std::make_unique<MXGen>(prio, name);
  }
  void toMessage(DNSMessageWriter& dpw) override;
  std::string toString() const override;
  DNSType getType() const override { return DNSType::MX; }
  uint16_t d_prio;
  DNSName d_name;
};

//! Generates an TXT Resource Record
struct TXTGen : RRGen
{
  TXTGen(const std::string& txt) : d_txt(txt) {}
  static std::unique_ptr<RRGen> make(const std::string& txt)
  {
    return std::make_unique<TXTGen>(txt);
  }
  void toMessage(DNSMessageWriter& dpw) override;
  std::string toString() const override;
  DNSType getType() const override { return DNSType::TXT; }
  std::string d_txt;
};

//! This implements 'unknown record types'
struct UnknownGen : RRGen
{
  UnknownGen(DNSType type, const std::string& rr) : d_type(type), d_rr(rr) {}
  DNSType d_type;
  std::string d_rr;
  void toMessage(DNSMessageWriter& dpw) override;
  std::string toString() const override;
  DNSType getType() const override { return d_type; }
};

//! This implements a fun dynamic TXT record type 
struct ClockTXTGen : RRGen
{
  ClockTXTGen(const std::string& format) : d_format(format) {}
  static std::unique_ptr<RRGen> make(const std::string& format)
  {
    return std::make_unique<ClockTXTGen>(format);
  }
  void toMessage(DNSMessageWriter& dpw) override;
  std::string toString() const { return d_format; }
  DNSType getType() const override { return DNSType::TXT; }
  std::string d_format;
};
