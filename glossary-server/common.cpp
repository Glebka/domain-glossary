#include <common.h>

QDataStream & operator <<(QDataStream &out, const TermInfo & ti)
{
    out<<TERM_INFO_START;
    out<<ti.id;
    out<<ti.domain_id;
    out<<ti.title;
    out<<ti.concept_list;
    out<<TERM_INFO_END;
    return out;
}

QDataStream & operator <<(QDataStream &out, const ConceptInfo & ci)
{
    out<<CONCEPT_INFO_START;
    out<<ci.id;
    out<<ci.domain_id;
    out<<(QList<quint32>)ci.term_list;
    out<<(QList<QString>)ci.keywords;
    out<<ci.last_modified;
    out<<CONCEPT_INFO_END;
    return out;
}

QDataStream & operator >>(QDataStream &out, TermInfo & ti)
{
    quint32 start_flag;
    quint32 stop_flag;

    out>>start_flag;
    out>>ti.id;
    out>>ti.domain_id;
    out>>ti.title;
    out>>ti.concept_list;
    out>>stop_flag;

    if(start_flag!=TERM_INFO_START || stop_flag!=TERM_INFO_END)
    {
        ti.id=0;
        qDebug()<<"Broken TermInfo Structure";
    }

    return out;
}

QDataStream & operator >>(QDataStream &out, ConceptInfo & ci)
{
    quint32 start_flag;
    quint32 stop_flag;

    out>>start_flag;
    out>>ci.id;
    out>>ci.domain_id;
    out>>ci.term_list;
    out>>ci.keywords;
    out>>ci.last_modified;
    out>>stop_flag;

    if(start_flag!=CONCEPT_INFO_START || stop_flag!=CONCEPT_INFO_END)
    {
        ci.id=0;
        qDebug()<<"Broken ConceptInfo Structure";
    }

    return out;
}

QDataStream &operator <<(QDataStream &out, const DomainInfo &di)
{
    out<<DOMAIN_INFO_START;
    out<<di.id;
    out<<di.title;
    out<<DOMAIN_INFO_END;
    qDebug()<<"Serialize";
    return out;
}


QDataStream &operator >>(QDataStream &out, DomainInfo &di)
{
    quint32 start_flag;
    quint32 stop_flag;

    out>>start_flag;
    out>>di.id;
    out>>di.title;
    out>>stop_flag;

    if(start_flag!=DOMAIN_INFO_START || stop_flag!=DOMAIN_INFO_END)
    {
        di.id=0;
        qDebug()<<"Broken DomainInfo Structure";
    }
    return out;
}


QDataStream &operator <<(QDataStream &out, const UserInfo &ui)
{
    out<<USER_INFO_START;
    out<<ui.id;
    out<<ui.domain_id;
    out<<(quint32)ui.type;
    out<<ui.email;
    out<<ui.password;
    out<<ui.full_name;
    out<<USER_INFO_END;
    return out;
}


QDataStream &operator >>(QDataStream &out, UserInfo &ui)
{
    quint32 start_flag;
    quint32 stop_flag;
    quint32 raw_type;

    out>>start_flag;
    out>>ui.id;
    out>>ui.domain_id;
    out>>raw_type;
    out>>ui.email;
    out>>ui.password;
    out>>ui.full_name;
    out>>stop_flag;

    ui.type=(UserType)raw_type;

    if(start_flag!=USER_INFO_START || stop_flag!=USER_INFO_END)
    {
        ui.id=0;
        qDebug()<<"Broken UserInfo Structure";
    }
    return out;
}


QDataStream &operator <<(QDataStream &out, const TermDefinition &td)
{
    out<<TERM_DEFINITION_START;
    out<<td.termInfo;
    out<<td.conceptInfo;
    out<<td.domain;
    out<<td.title;
    out<<td.keywords;
    out<<td.text;
    out<<TERM_DEFINITION_END;
    return out;
}


QDataStream &operator >>(QDataStream &out, TermDefinition &td)
{
    quint32 start_flag;
    quint32 stop_flag;

    out>>start_flag;
    out>>td.termInfo;
    out>>td.conceptInfo;
    out>>td.domain;
    out>>td.title;
    out>>td.keywords;
    out>>td.text;
    out>>stop_flag;

    if(start_flag!=TERM_DEFINITION_START || stop_flag!=TERM_DEFINITION_END)
    {
        td.termInfo.id=0;
        td.conceptInfo.id=0;
        qDebug()<<"Broken TermDefinition Structure";
    }

    return out;
}


QDataStream &operator <<(QDataStream &out, const PacketHeader &ph)
{
    out<<ph.start_flag;
    out<<ph.uid;
    out<<ph.command;
    out<<ph.status;
    out<<ph.data_length;
    return out;
}


QDataStream &operator >>(QDataStream &out, PacketHeader &ph)
{
    out>>ph.start_flag;
    out>>ph.uid;
    out>>ph.command;
    out>>ph.status;
    out>>ph.data_length;
    return out;
}
