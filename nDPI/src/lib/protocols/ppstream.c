/*
 * ppstream.c
 *
 * Copyright (C) 2009-2011 by ipoque GmbH
 * Copyright (C) 2011-13 - ntop.org
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
#ifdef NDPI_PROTOCOL_PPSTREAM

static void ndpi_int_ppstream_add_connection(struct ndpi_detection_module_struct
											   *ndpi_struct, struct ndpi_flow_struct *flow)
{
	ndpi_int_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_PPSTREAM, NDPI_REAL_PROTOCOL);
}

void ndpi_search_ppstream(struct ndpi_detection_module_struct
							*ndpi_struct, struct ndpi_flow_struct *flow)
{
	struct ndpi_packet_struct *packet = &flow->packet;
	
    u_int16_t len = packet->payload_packet_len;
    u_int16_t counter = flow->packet_counter;
	// struct ndpi_id_struct *src=ndpi_struct->src;
	// struct ndpi_id_struct *dst=ndpi_struct->dst;



	/* check TCP Connections -> Videodata */
	if (packet->tcp != NULL) {
		if (packet->payload_packet_len >= 60 && get_u_int32_t(packet->payload, 52) == 0
			&& memcmp(packet->payload, "PSProtocol\x0", 11) == 0) {
			NDPI_LOG(NDPI_PROTOCOL_PPSTREAM, ndpi_struct, NDPI_LOG_DEBUG, "found ppstream over tcp.\n");
			ndpi_int_ppstream_add_connection(ndpi_struct, flow);
			return;
		}
        //0x7300000001000000 bf00b5ac
        NDPI_LOG(NDPI_PROTOCOL_PPSTREAM, ndpi_struct, NDPI_LOG_DEBUG, "ppstream len:%x,payload[0]:%x.\n",len,packet->payload[0]);
        if(len == packet->payload[0]
        && packet->payload[4] == 0x01
        && len > 8+ 6
        ){
            u_int16_t i = len;
            for(i;i>0;i--){
                if(packet->payload[i] == 0x00 && packet->payload[i-1] == 0x1a){
                    if(memcmp(&packet->payload[i+1],"\x2f\x76\x69\x64\x65\x6f\x73",7)==0){
			            NDPI_LOG(NDPI_PROTOCOL_PPSTREAM, ndpi_struct, NDPI_LOG_DEBUG, "found ppstream over tcp.\n");
			            ndpi_int_ppstream_add_connection(ndpi_struct, flow);
		        	    return;
                    }else{
                       goto exit; 
                    }
                }
                continue;
            }
        }
	}

	if (packet->udp != NULL) {
		if (packet->payload_packet_len > 2 && packet->payload[2] == 0x43
			&& ((packet->payload_packet_len - 4 == get_l16(packet->payload, 0))
				|| (packet->payload_packet_len == get_l16(packet->payload, 0))
				|| (packet->payload_packet_len >= 6 && packet->payload_packet_len - 6 == get_l16(packet->payload, 0)))) {
			flow->l4.udp.ppstream_stage++;
			if (flow->l4.udp.ppstream_stage == 5) {
				NDPI_LOG(NDPI_PROTOCOL_PPSTREAM, ndpi_struct, NDPI_LOG_DEBUG,
						"found ppstream over udp pattern len, 43.\n");
				ndpi_int_ppstream_add_connection(ndpi_struct, flow);
				return;
			}
			return;
		}
        //udp---特征【len】8e 80 ca db db cc 98 85
        //新闻短视频流
        NDPI_LOG(NDPI_PROTOCOL_PPSTREAM, ndpi_struct, NDPI_LOG_DEBUG, "ppstream len:%x,payload[0]:%x.\n",len,packet->payload[0]);
        if(len >=16 
        && packet->payload[1] == 0x80
        && packet->payload[3] == packet->payload[4]
        && (packet->payload[12] == packet->payload[13]+1
            || packet->payload[12] == packet->payload[13]-1
            || packet->payload[12] == packet->payload[13])
        && packet->payload[13] == packet->payload[14]
        ){
            if(len == packet->payload[0] || packet->payload[0] == 0x14){
				NDPI_LOG(NDPI_PROTOCOL_PPSTREAM, ndpi_struct, NDPI_LOG_DEBUG,
						"found ppstream over udp.\n");
				ndpi_int_ppstream_add_connection(ndpi_struct, flow);
				return;
            }
        }
        //udp --特征 14 80 50 13 13 42 19 13 。。。。
        //该特征为udp发送第二个数据包
	}
#if 0
		/* wanglei 2016-10-18 11:40 */
		if (packet->payload_packet_len > 2 && packet->payload[0] == 0x43) {
				NDPI_LOG(NDPI_PROTOCOL_PPSTREAM, ndpi_struct, NDPI_LOG_DEBUG,
						"found ppstream over udp pattern len, 43.\n");
				ndpi_int_ppstream_add_connection(ndpi_struct, flow);
				return;
		}

		if (flow->l4.udp.ppstream_stage == 0
			&& packet->payload_packet_len > 4 && ((packet->payload_packet_len - 4 == get_l16(packet->payload, 0))
												  || (packet->payload_packet_len == get_l16(packet->payload, 0))
												  || (packet->payload_packet_len >= 6
													  && packet->payload_packet_len - 6 == get_l16(packet->payload,
																								   0)))) {

			if (packet->payload[2] == 0x00 && packet->payload[3] == 0x00 && packet->payload[4] == 0x03) {
				flow->l4.udp.ppstream_stage = 7;
				NDPI_LOG(NDPI_PROTOCOL_PPSTREAM, ndpi_struct, NDPI_LOG_DEBUG, "need next packet I.\n");
				return;
			}
		}

		if (flow->l4.udp.ppstream_stage == 7
			&& packet->payload_packet_len > 4 && packet->payload[3] == 0x00
			&& ((packet->payload_packet_len - 4 == get_l16(packet->payload, 0))
				|| (packet->payload_packet_len == get_l16(packet->payload, 0))
				|| (packet->payload_packet_len >= 6 && packet->payload_packet_len - 6 == get_l16(packet->payload, 0)))
			&& (packet->payload[2] == 0x00 && packet->payload[4] == 0x03)) {
			NDPI_LOG(NDPI_PROTOCOL_PPSTREAM, ndpi_struct, NDPI_LOG_DEBUG,
					"found ppstream over udp with pattern Vb.\n");
			ndpi_int_ppstream_add_connection(ndpi_struct, flow);
			return;
		}



#endif
exit:
	NDPI_LOG(NDPI_PROTOCOL_PPSTREAM, ndpi_struct, NDPI_LOG_DEBUG, "exclude ppstream.\n");
	NDPI_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, NDPI_PROTOCOL_PPSTREAM);
}
#endif
