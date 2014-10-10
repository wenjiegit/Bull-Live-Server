
/*
 * Copyright (C) wenjie.zhao
 */


#ifndef MCONFPARSER_HPP
#define MCONFPARSER_HPP

#include <vector>
#include "mstring.hpp"
#include "mstringlist.hpp"

enum MEE_TYPE {
    UNKNOWN = -1,
    LINE    = 0,
    BLOCK_START,
    BLOCK_END,
    Comment
};

// MEE  Element
class MEE
{
public:
    MEE();

    bool parse(MStringList &lines, int &lineNo);
    int readToken(MString &line, int lineNo, MEE *mee);
    void setValue(const MString &key, const MStringList &value);
    void setValue(const MString &key, const MString &value);
    void setBlock(const MString &key, const MStringList &value);

    MString arg0();
    MString arg1();
    MString arg2();
    MString arg3();

    inline MStringList arguments() { return args; }
    inline vector<MEE*> allLines() { return ees; }

    MString dump();
    MString dumpWithTab(int &tabIndex);

    // simple get element
    MEE *get(const MString &name1);
    vector<MEE*> getVector(const MString &name1);

    MEE *get(const MString &name1, const MString &name2);
    vector<MEE*> getVector(const MString &name1, const MString &name2);

    MEE *get(const MString &name1, const MString &name2, const MString &name3);
    vector<MEE*> getVector(const MString &name1, const MString &name2, const MString &name3);

public:
    vector<MEE*> ees;
    MStringList args;
    MEE_TYPE type;
    muint32 lineNo;
    MString name;

public:
    friend ostream & operator<<(ostream &o, const MEE& a);
    friend MEE *get(const MString &name, const MString &arg, const vector<MEE*> &mees);
};

class MConf
{
public:
    MConf(const MString & name);
    ~MConf();

    inline MEE *root() { return m_root; }
    bool parse(MString &cont);

private:
    MString readFile(const MString &name);

    bool write();

private:
    MEE *m_root;
};

#endif // MCONFPARSER_HPP
