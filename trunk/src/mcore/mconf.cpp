
/*
 * Copyright (C) wenjie.zhao
 */


#include "mconf.hpp"

#include "mfile.hpp"
#include "mloger.hpp"

#define Read_LINE           0
#define Read_Block_Start    1
#define Read_Block_End      2
#define Read_Comment        3
#define Read_ERROR          -1

MEE::MEE()
    : type(UNKNOWN)
    , lineNo(-1)
{
}

bool MEE::parse(MStringList &lines, int &lineNo)
{
    while (lines.size() > 0) {
        ++lineNo;
        MEE *ee = new MEE;
        ee->lineNo = lineNo;

        ees.push_back(ee);
        MString line = lines.front();

        int ret = readToken(line, lineNo, ee);
        if (ret == Read_ERROR) {
            mAssert(ret != Read_ERROR);
            return false;
        }

        if (ret == Read_Comment) {
            ee->type = Comment;
        } else if (ret == Read_LINE) {
        } else if (ret == Read_Block_Start) {
            lines.erase(lines.begin());
            ee->parse(lines, lineNo);
            continue;
        } else if (ret == Read_Block_End) {
            lines.erase(lines.begin());
            return true;
        }

        lines.erase(lines.begin());
    }

    return true;
}

int MEE::readToken(MString &line, int lineNo, MEE *mee)
{
    line = line.trimmed();

    if (line.startWith("#")) {
        mee->name = "#";
        mee->args << line;
        mee->type = Comment;
        return Read_Comment;
    }

    if (line == "}") {
        mee->name = "}";
        mee->type = BLOCK_END;
        return Read_Block_End;
    }

    MStringList ee = line.split(" ");
    if (ee.size() <= 1) {
        log_error("line %d has no argument.", lineNo);
        return Read_ERROR;
    }

    mee->name = ee.front();
    ee.pop_front();

    if (ee.back() == "{") {
        mee->type = BLOCK_START;
        ee.pop_back();
        mee->args << ee;

        return Read_Block_Start;
    }
    mee->args << ee;

    // line parse
    if (!ee.back().endWith(";")) {
        log_error("line %d expect \";\"", lineNo);
        return Read_ERROR;
    }
    mee->type = LINE;

    // fix last ;
    MString &bk = mee->args.back();
    if (bk.endWith(";")) {
        bk.erase(bk.size()-1, 1);
    }

    return Read_LINE;
}

void MEE::setValue(const MString &key, const MStringList &value)
{
    MEE *v = new MEE;
    v->type = LINE;
    v->name = key;
    v->args = value;

    ees.push_back(v);
}

void MEE::setValue(const MString &key, const MString &value)
{
    MStringList args;
    args << value;
    setValue(key, args);
}

MString MEE::arg0()
{
    if (args.size() >= 1) {
        return args.front();
    }

    return "";
}

MString MEE::arg1()
{
    if (args.size() >= 2) {
        return args.at(1);
    }

    return "";
}

MString MEE::arg2()
{
    if (args.size() >= 3) {
        return args.at(2);
    }

    return "";
}

MString MEE::arg3()
{
    if (args.size() >= 4) {
        return args.at(3);
    }

    return "";
}

MString MEE::dump()
{
    int tabIndex = 0;
    return dumpWithTab(tabIndex);
}

MString MEE::dumpWithTab(int &tabIndex)
{
    MString ret;
    for (unsigned int i = 0; i < ees.size(); ++i) {
        MEE *ee = ees.at(i);
        if (ee->type == Comment) {
            ret.sprintf("%s\n", ee->args.join(" ").c_str());
        } else if (ee->type == LINE) {
            ret.sprintf("%s%s %s;\n", MString(tabIndex, '\t').c_str(), ee->name.c_str(), ee->args.join(" ").c_str());
        } else if (ee->type == BLOCK_START) {
            ret.sprintf("%s%s %s {\n", MString(tabIndex, '\t').c_str(), ee->name.c_str(), ee->args.join(" ").c_str());
            ret << ee->dumpWithTab(++tabIndex);
        } else if (ee->type == BLOCK_END) {\
             ret.sprintf("%s }\n", MString(--tabIndex, '\t').c_str());
        }
    }

    return ret;
}

MEE *MEE::get(const MString &name1)
{
    MEE *ret = NULL;
    vector<MEE *> e = getVector(name1);
    if (e.size() > 0) {
        ret = e.front();
    }

    return ret;
}

vector<MEE *> MEE::getVector(const MString &name1)
{
    vector<MEE *> ret;
    for (unsigned int i = 0; i < ees.size(); ++i) {
        MEE *ee = ees.at(i);
        if (ee && ee->name == name1) {
            ret.push_back(ee);
        }
    }
    return ret;
}

MEE *MEE::get(const MString &name1, const MString &name2)
{
    MEE *ret = NULL;
    vector<MEE *> e = getVector(name1, name2);
    if (e.size() > 0) {
        ret = e.front();
    }

    return ret;
}

vector<MEE *> MEE::getVector(const MString &name1, const MString &name2)
{
    vector<MEE *> ret;
    vector<MEE *> L1 = getVector(name1);
    for (unsigned int i = 0; i < L1.size(); ++i) {
        MEE *e1 = L1.at(i);
        vector<MEE *> ees = e1->ees;
        for (unsigned int j = 0; j < ees.size(); ++j) {
            MEE *e2 = ees.at(j);
            if (e2 && e2->name == name2) {
                ret.push_back(e2);
            }
        }
    }

    return ret;
}

MEE *MEE::get(const MString &name1, const MString &name2, const MString &name3)
{
    MEE *ret = NULL;
    vector<MEE *> e = getVector(name1, name2, name3);
    if (e.size() > 0) {
        ret = e.front();
    }

    return ret;
}

vector<MEE *> MEE::getVector(const MString &name1, const MString &name2, const MString &name3)
{

    vector<MEE *> ret;
    vector<MEE *> L2 = getVector(name1, name2);
    for (unsigned int i = 0; i < L2.size(); ++i) {
        MEE *e2 = L2.at(i);
        vector<MEE *> ees = e2->ees;
        for (unsigned int j = 0; j < ees.size(); ++j) {
            MEE *e2 = ees.at(j);
            if (e2 && e2->name == name3) {
                ret.push_back(e2);
            }
        }
    }

    return ret;
}

ostream & operator<<(ostream &o, const MEE& a)
{
    MEE &b = const_cast<MEE &>(a);
    o << b.dump();

    return o;
}

MEE *get(const MString &name, const MString &arg, const vector<MEE *> &mees)
{
    for (unsigned int i = 0; i < mees.size(); ++i) {
       MEE *ee = mees.at(i);
       if (ee && ee->name == name && ee->arg0() == arg) {
           return ee;
       }
    }

    return NULL;
}

MConf::MConf(const MString &name)
    : m_root(NULL)
{
    MString cont = readFile(name);
    if (!parse(cont)) {
        log_error("parse conf file open error.");
    }
}

MConf::~MConf()
{
}

MString MConf::readFile(const MString &name)
{
    MString ret;

    MFile *f = new MFile(name);
    mAutoFree(MFile, f);

    if (f->open("r")) {
        ret = f->readAll();
    } else {
        log_error("conf file open error.");
    }

    return ret;
}

bool MConf::parse(MString &cont)
{
    m_root = new MEE;
    m_root->type = BLOCK_START;
    MStringList lines = cont.split("\n");

    int lineNo = 0;
    return m_root->parse(lines, lineNo);
}
