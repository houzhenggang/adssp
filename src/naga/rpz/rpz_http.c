#include "rpz.h"

void hytag_mac_rever(hytag_t *tag)
{
    mac_t smac, dmac;

    maccpy(smac, tag->srcmac);
    maccpy(dmac, tag->dstmac);


    maccpy(tag->srcmac, smac);
    maccpy(tag->dstmac, dmac);
}


void hytag_ip_rever(hytag_t *tag)
{
    uint16_t sip, dip;

    struct tcp_hdr *tcphdr = NULL;

    sip = tag->outer_srcip;
    dip = tag->outer_dstip;

    tag->outer_srcip = sip;
    tag->outer_dstip = dip;

    hytag_mac_rever(tag);
}

void hytag_tcp_rever(hytag_t *tag)
{
    uint16_t sport, dport;
    uint32_t seq, ack;

    sport = tag->outer_srcport;
    dport = tag->outer_dstport;

    tag->outer_srcport = dport;
    tag->outer_dstport = sport;

    seq = tag->outer_seq;
    ack = tag->outer_ack;

    tag->outer_seq = ack;
    tag->outer_ack = seq;

    hytag_ip_rever(tag);
}

berr rpz_tcp_send_pkt(hytag_t *tag, char *payload, int paylen, int offset)
{
    struct tcp_hdr *tcphdr = NULL;

    tcphdr = tag->l4_offset;

    tcphdr->src_port = tag->outer_srcport;
    tcphdr->dst_port = tag->outer_dstport;

    tcphdr->sent_seq = tag->outer_seq;
    tcphdr->recv_ack = tag->outer_ack;

    return rpz_ip_send_pkt(hytag_t *tag, char *payload, int paylen);
}

berr rpz_tcp_send(hytag_t *tag, char *connect, int connect_len)
{
    int offset = 0;
    char *cp = NULL;

    while (offset < connect_len)
    {
        if (0 == offset)
        {
            clen = HTTP_HEAD_SZ;
        }
        else
        {
            clen = HTTP_CONNECT_UNIT_SZ;
        }

        cp = connect + offset;
        ERR_RETURN(rpz_tcp_send_pkt(&tag, cp, clen, offset));

        offset += clen;
    }
}

berr rpz_http_respose(hytag_t *raw, char *connect, int connect_len)
{

    hytag_t tag;

    memset(&tag, 0, sizeof(hytag_t));

    memcpy(tag, raw, sizeof(hytag_t));

    hytag_tcp_rever(&tag);

    return rpz_tcp_send(tag,connect,connect_len);
}

/* End of file */
