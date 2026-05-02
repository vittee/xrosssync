#pragma once

#include "xr18/nodes/Node.h"
#include "xr18/nodes/Nodes.h"
#include "xr18/nodes/params/StringParam.h"
#include "xr18/nodes/params/IntParam.h"
#include "xr18/nodes/params/EnumParam.h"

namespace xr18 {
namespace nodes {
namespace channels
{

static const std::vector<const char*> kInSrcNames = { "In01", "In02", "In03", "In04", "In05", "In06", "In07", "In08", "In09", "In10", "In11", "In12", "In13", "In14", "In15", "In16", "L", "R", "OFF" };

static const std::vector<const char*> kRtnSrcNames = { "U01", "U02", "U03", "U04", "U05", "U06", "U07", "U08", "U09", "U10", "U11", "U12", "U13", "U14", "U15", "U16", "U17", "U18" };

class Config : public Node {
public:
    Config(Node* parent)
        :
        Node("config", parent),
        param_name("name", this),
        param_color("color", this, 0, 15)
    {

    }
private:
    params::StringParam param_name;
    params::IntParam param_color;
};

class InputConfig : public Config {
public:
    InputConfig(Node* parent)
        :
        Config(parent),
        param_insrc("insrc", this, kInSrcNames),
        param_rtnsrc("rtnsrc", this, kRtnSrcNames)
    {

    }
private:
    params::EnumParam param_insrc;
    params::EnumParam param_rtnsrc;
};

class ReturnConfig : public Config {
public:
    ReturnConfig(Node* parent)
        :
        Config(parent),
        param_rtnsrc("rtnsrc", this, kRtnSrcNames)
    {

    }
private:
    params::EnumParam param_rtnsrc;
};


} // namespace channels
} // namespace nodes
} // namespace xr18
