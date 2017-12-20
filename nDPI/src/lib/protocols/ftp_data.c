/*
 * ftp_data.c
 *
 * Copyright (C) 2016 - ntop.org
 *
 * The signature is based on the Libprotoident library.
 *
 * This file is part of nDPI, an open source deep packet inspection
 * library based on the OpenDPI and PACE technology by ipoque GmbH
 *
 * nDPI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * nDPI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with nDPI.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ndpi_protocols.h"
#include "ndpi_utils.h"

#ifdef NDPI_PROTOCOL_FTP_DATA
static void ndpi_int_ftp_data_add_connection(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow) {
    //ndpi_set_detected_protocol(ndpi_struct, flow, NDPI_PROTOCOL_FTP_DATA, NDPI_PROTOCOL_UNKNOWN);
    ndpi_int_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_FTP_DATA, NDPI_PROTOCOL_UNKNOWN);
}

static int ndpi_match_ftp_data_port(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow) {
    struct ndpi_packet_struct *packet = &flow->packet;

    /* Check connection over TCP */
    if(packet->tcp) {
        if(packet->tcp->dest == htons(20) || packet->tcp->source == htons(20)) {
            return 1;
        }
    }
    return 0;
}

static int ndpi_match_ftp_data_directory(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow) {
    struct ndpi_packet_struct *packet = &flow->packet;
    u_int32_t payload_len = packet->payload_packet_len;

    if((payload_len >= 4)
            && ((packet->payload[0] == '-') || (packet->payload[0] == 'd'))
            && ((packet->payload[1] == '-') || (packet->payload[1] == 'r'))
            && ((packet->payload[2] == '-') || (packet->payload[2] == 'w'))
            && ((packet->payload[3] == '-') || (packet->payload[3] == 'x'))) {

        return 1;
    }

    return 0;
}

static int ndpi_match_file_header(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow) {
    struct ndpi_packet_struct *packet = &flow->packet;
    u_int32_t payload_len = packet->payload_packet_len;

    /* A FTP packet is pretty long so 256 is a bit conservative but it should be OK */
    if(packet->payload_packet_len < 80)
        return 0;

    /* RIFF is a meta-format for storing AVI and WAV files */
    if(ndpi_match_strprefix(packet->payload, payload_len, "RIFF"))
        return 1;

    /* MZ is a .exe file */
    if((packet->payload[0] == 'M') && (packet->payload[1] == 'Z') && (packet->payload[3] == 0x00))
        return 1;

    /* Ogg files */
    if(ndpi_match_strprefix(packet->payload, payload_len, "OggS"))
        return 1;

    /* ZIP files */
    if((packet->payload[0] == 'P') && (packet->payload[1] == 'K') && (packet->payload[2] == 0x03) && (packet->payload[3] == 0x04))
        return 1;

    /* MPEG files */
    if((packet->payload[0] == 0x00) && (packet->payload[1] == 0x00) && (packet->payload[2] == 0x01) && (packet->payload[3] == 0xba))
        return 1;

    /* RAR files */
    if(ndpi_match_strprefix(packet->payload, payload_len, "Rar!"))
        return 1;

    /* EBML */
    if((packet->payload[0] == 0x1a) && (packet->payload[1] == 0x45) && (packet->payload[2] == 0xdf) && (packet->payload[3] == 0xa3))
        return 1;

    /* JPG */
    if((packet->payload[0] == 0xff) && (packet->payload[1] ==0xd8))
        return 1;

    /* GIF */
    if(ndpi_match_strprefix(packet->payload, payload_len, "GIF8"))
        return 1;

    /* PHP scripts */
    if((packet->payload[0] == 0x3c) && (packet->payload[1] == 0x3f) && (packet->payload[2] == 0x70) && (packet->payload[3] == 0x68))
        return 1;

    /* Unix scripts */
    if((packet->payload[0] == 0x23) && (packet->payload[1] == 0x21) && (packet->payload[2] == 0x2f) && (packet->payload[3] == 0x62))
        return 1;

    /* PDFs */
    if(ndpi_match_strprefix(packet->payload, payload_len, "%PDF"))
        return 1;

    /* PNG */
    if((packet->payload[0] == 0x89) && (packet->payload[1] == 'P') && (packet->payload[2] == 'N') && (packet->payload[3] == 'G'))
        return 1;

    /* HTML */
    if(ndpi_match_strprefix(packet->payload, payload_len, "<htm"))
        return 1;
    if((packet->payload[0] == 0x0a) && (packet->payload[1] == '<') && (packet->payload[2] == '!') && (packet->payload[3] == 'D'))
        return 1;

    /* 7zip */
    if((packet->payload[0] == 0x37) && (packet->payload[1] == 0x7a) && (packet->payload[2] == 0xbc) && (packet->payload[3] == 0xaf))
        return 1;

    /* gzip */
    if((packet->payload[0] == 0x1f) && (packet->payload[1] == 0x8b) && (packet->payload[2] == 0x08))
        return 1;

    /* XML */
    if(ndpi_match_strprefix(packet->payload, payload_len, "<!DO"))
        return 1;

    /* FLAC */
    if(ndpi_match_strprefix(packet->payload, payload_len, "fLaC"))
        return 1;

    /* MP3 */
    if((packet->payload[0] == 'I') && (packet->payload[1] == 'D') && (packet->payload[2] == '3') && (packet->payload[3] == 0x03))
        return 1;
    if(ndpi_match_strprefix(packet->payload, payload_len, "\xff\xfb\x90\xc0"))
        return 1;

    /* RPM */
    if((packet->payload[0] == 0xed) && (packet->payload[1] == 0xab) && (packet->payload[2] == 0xee) && (packet->payload[3] == 0xdb))
        return 1;

    /* Wz Patch */
    if(ndpi_match_strprefix(packet->payload, payload_len, "WzPa"))
        return 1;

    /* Flash Video */
    if((packet->payload[0] == 'F') && (packet->payload[1] == 'L') && (packet->payload[2] == 'V') && (packet->payload[3] == 0x01))
        return 1;

    /* Ref: file command and magic */
    if (!memcmp(packet->payload+4, "ftyp", 4)
            && (!memcmp(packet->payload+8, "iso", 3)       /* MPEG v4 */
                || !memcmp(packet->payload+8, "mp4", 3)))  /* MPEG v4 */
    {
        return 1;
    }

    /* .BKF (Microsoft Tape Format) */
    if(ndpi_match_strprefix(packet->payload, payload_len, "TAPE"))
        return 1;

    /* MS Office Doc file - this is unpleasantly geeky */
    if((packet->payload[0] == 0xd0) && (packet->payload[1] == 0xcf) && (packet->payload[2] == 0x11) && (packet->payload[3] == 0xe0))
        return 1;

    /* ASP */
    if((packet->payload[0] == 0x3c) && (packet->payload[1] == 0x25) && (packet->payload[2] == 0x40) && (packet->payload[3] == 0x20))
        return 1;

    /* WMS file */
    if((packet->payload[0] == 0x3c) && (packet->payload[1] == 0x21) && (packet->payload[2] == 0x2d) && (packet->payload[3] == 0x2d))
        return 1;

    /* ar archive, typically .deb files */
    if(ndpi_match_strprefix(packet->payload, payload_len, "!<ar"))
        return 1;

    /* Raw XML (skip jabber-like traffic as this is not FTP but unencrypted jabber) */
    if((ndpi_match_strprefix(packet->payload, payload_len, "<?xm"))
            && (ndpi_strnstr((const char *)packet->payload, "jabber", packet->payload_packet_len) == NULL))
        return 1;

    if(ndpi_match_strprefix(packet->payload, payload_len, "<iq "))
        return 1;

    /* SPF */
    if(ndpi_match_strprefix(packet->payload, payload_len, "SPFI"))
        return 1;

    /* ABIF - Applied Biosystems */
    if(ndpi_match_strprefix(packet->payload, payload_len, "ABIF"))
        return 1;

    /* bzip2 - other digits are also possible instead of 9 */
    if((packet->payload[0] == 'B') && (packet->payload[1] == 'Z') && (packet->payload[2] == 'h') && (packet->payload[3] == '9'))
        return 1;

    /* Rar archive, Ref: file command sources */
    if (!memcmp(packet->payload, "Rar!", 4) && (packet->payload[35] < 4))   /* 0: MS-DOS, 1: OS/2, 2: Win32, 3: Unix */
        return 1;
    /*
     * Zip archives, Ref: file command sources
     * 0x09: v0.9, 0x0a: v1.0, 0x0b: v1.1
     */
    if (!memcmp(packet->payload, "PK\003\004", 4) && (packet->payload[4] <= 0x0b))
        return 1;

    /* Some other types of files */

    if((packet->payload[0] == '<') && (packet->payload[1] == 'c') && (packet->payload[2] == 'f'))
        return 1;
    if((packet->payload[0] == '<') && (packet->payload[1] == 'C') && (packet->payload[2] == 'F'))
        return 1;
    if(ndpi_match_strprefix(packet->payload, payload_len, ".tem"))
        return 1;
    if(ndpi_match_strprefix(packet->payload, payload_len, ".ite"))
        return 1;
    if(ndpi_match_strprefix(packet->payload, payload_len, ".lef"))
        return 1;

    return 0;
}

static void ndpi_check_ftp_data(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow)
{
    struct ndpi_packet_struct *packet = &flow->packet;
    int found_pro;
    int ipsize = sizeof(u_int32_t);
    int offset;
    /* client ip, client port, server ip, server port, tcp/udp */
    u_int8_t key_buff[2*sizeof(u_int32_t) + 2*2 + 1] = {0};     /* set zero */

    /* it must be tcp stream */
    if (!packet->tcp) return;
    /* TODO ftp_data: support ipv6 */
    /* Don't support ipv6 now */
    if (!packet->iph) return;

    offset = ipsize+2;
    memcpy(key_buff+offset, &packet->iph->saddr, ipsize);       /* server ip */
    offset += ipsize;
    memcpy(key_buff+offset, &packet->tcp->source, 2);           /* server port */
    offset += 2;
    key_buff[offset] = IPPROTO_TCP;                             /* tcp or udp */
    offset += 1;

    /* find and remove this record, if it exists. */
    /* NOTE maybe need to add lock and change `remove' to `search' then invoke `remove' */
    found_pro = ndpi_hash_remove(ndpi_struct->meta2protocol, key_buff, offset, NDPI_PROTOCOL_FTP_DATA);
    if (found_pro) {
        NDPI_LOG(NDPI_PROTOCOL_FTP_DATA, ndpi_struct, NDPI_LOG_DEBUG, "Found FTP_DATA via server-port record ...\n");
        ndpi_int_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_FTP_DATA, NDPI_REAL_PROTOCOL);
        return;
    }

    if((packet->payload_packet_len > 0)
            && (ndpi_match_file_header(ndpi_struct, flow)
                || ndpi_match_ftp_data_directory(ndpi_struct, flow)
                || ndpi_match_ftp_data_port(ndpi_struct, flow)
               )) {
        NDPI_LOG(NDPI_PROTOCOL_FTP_DATA, ndpi_struct, NDPI_LOG_DEBUG, "Possible FTP_DATA request detected...\n");
        ndpi_int_ftp_data_add_connection(ndpi_struct, flow);
    } else
        NDPI_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, NDPI_PROTOCOL_FTP_DATA);
}

void ndpi_search_ftp_data(struct ndpi_detection_module_struct *ndpi_struct, struct ndpi_flow_struct *flow)
{
    /* Break after 20 packets. */
    if(flow->packet_counter > 20) {
        NDPI_LOG(NDPI_PROTOCOL_FTP_DATA, ndpi_struct, NDPI_LOG_DEBUG, "Exclude FTP_DATA.\n");
        NDPI_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, NDPI_PROTOCOL_FTP_DATA);
        return;
    }

    NDPI_LOG(NDPI_PROTOCOL_FTP_DATA, ndpi_struct, NDPI_LOG_DEBUG, "FTP_DATA detection...\n");
    ndpi_check_ftp_data(ndpi_struct, flow);
}

#endif