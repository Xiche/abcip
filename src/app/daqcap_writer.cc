//--------------------------------------------------------------------- SOL
// This file is part of abcip, a simple packet crafting tool.
// Copyright (C) 2016 Michael R. Altizer
//
// Abcip is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your
// option) any later version.
// 
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//--------------------------------------------------------------------- EOL

//-------------------------------------------------------------------------
// daqcap writer stuff
//-------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include <daq_capture.h>
#include <sfbpf_dlt.h>
}

#include "cake.h"
#include "daqcap_writer.h"

#define MAX_SNAP 65535

class DaqcapWriterImpl
{
public:
    DAQ_Capture_t *capture;
    unsigned numPkts;
    struct timeval ptime;
    struct timeval last;
};

//-------------------------------------------------------------------------
// this is a bit of a hack but covers the most common cases.
//
// * the root proto determines the dlt which is stored in
//   the file so changing that on the fly will cause broken
//   pcaps.
// 
// * pcap doesn't like DLT_IPV? in a file, giving:
//   "link-layer type -1 isn't supported in savefiles"
//   so we return raw instead.

static int GetDataLinkType(const char *proto)
{
    if (!strncasecmp(proto, "eth", 3))
        return DLT_EN10MB;

#if 0
    if (!strncasecmp(proto, "ip4", 3))
        return DLT_IPV4;

    if (!strncasecmp(proto, "ip6", 3))
        return DLT_IPV6;
#endif

    return DLT_RAW;
}

//-------------------------------------------------------------------------

DaqcapWriter::DaqcapWriter(const char *name, const char *root, uint32_t max)
{
    my = new DaqcapWriterImpl;

    if (!name)
        return;

    if (!max)
        max = MAX_SNAP;
    int dlt = GetDataLinkType(root);

    my->capture = daq_capture_open_writer(name, dlt, max);
    if (!my->capture)
        cerr << "Error - can't open DAQ capture file" << endl;
    my->numPkts = 0;

    gettimeofday(&my->ptime, NULL);
    my->last = my->ptime;
}

DaqcapWriter::~DaqcapWriter()
{
    if (my->capture)
        daq_capture_close(my->capture);
    cout << my->numPkts << " packets written" << endl;
    delete my;
}

bool DaqcapWriter::Ok()
{
    return my->capture != NULL;
}

void DaqcapWriter::operator<<(const Packet & p)
{
    if (!my->capture)
        return;

    DAQ_PktHdr_t h;

    h = p.daqhdr;
    h.pktlen = p.Length();
    h.caplen = (p.snap && h.pktlen > p.snap) ? p.snap : h.pktlen;

    printf("Writer - %d(%d) -> %d(%d) AS%hu\n", h.ingress_index, h.ingress_group,
            h.egress_index, h.egress_group, h.address_space_id);

    struct timeval now;
    gettimeofday(&now, NULL);

    long ds, du;

    if (p.late)
    {
        ds = (long) p.late;
        du = round((p.late - ds) * 1e6);
    }
    else
    {
        ds = now.tv_sec - my->last.tv_sec;
        du = now.tv_usec - my->last.tv_usec;

        if (ds < 0)
            ds = 0;
        if (du < 0)
            du += 1000000;

    }
    my->ptime.tv_sec += ds;
    my->ptime.tv_usec += du;

    if (my->ptime.tv_usec > 1000000)
    {
        my->ptime.tv_usec -= 1000000;
        my->ptime.tv_sec++;
    }
    h.ts.tv_sec = my->ptime.tv_sec;
    h.ts.tv_usec = my->ptime.tv_usec;

    daq_capture_write(my->capture, &h, p.Data());

    my->numPkts++;
    my->last = now;
}
