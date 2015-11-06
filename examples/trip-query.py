#!/usr/bin/python
# vim: set fileencoding=utf-8 :
#
# Query trips between the two locations given on the command line
#
# Copyright (C) 2014 Guido Günther
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

import sys
import optparse

from gi import require_version

require_version('Lpf', '0.0')
from gi.repository import Lpf
from gi.repository import GObject
from gi.repository import GLib

mainloop = None
start = None
end = None
provider = None
options = None

def quit(error=None):
    if error:
        print("Error: %s" % error)
    mainloop.quit()


def locs_cb(locs, userdata, err):
    global start, end

    if err:
        quit(err.message)
        return

    if not start:
        start = locs[0]
        end, when = userdata
        print("Start: %s" % start.props.name)
        provider.get_locs(end, 0, locs_cb, when)
    else:
        end = locs[0]
        print("End: %s" % end.props.name)
        dt = GLib.DateTime.new_local(*userdata) if userdata else GLib.DateTime.new_now_local()
        provider.get_trips(start, end, dt, 0, trips_cb, None)

def format_full(trips):
    i = 0
    for trip in trips:
        i += 1
        j = 0
        msg = ' - CANCELED' if trip.props.status == Lpf.TripStatusFlags.CANCELED else ''
        print ('Trip #%d%s' % (i, msg))
        for part in trip.props.parts:
            j += 1
            print('   Part #%d' % j)
            start = part.props.start
            end = part.props.end
            print("       Start:     %s" % start.props.name)
            print("       Departure: %s" % start.props.departure.format("%F %H:%M"))
            if start.props.dep_plat:
                print("       Platform:  %s" % start.props.dep_plat)
            if start.props.rt_departure:
                print("       Delay:     %s" % start.props.departure_delay)
            print("       End:       %s" % end.props.name)
            print("       Arrival:   %s" % end.props.arrival.format("%F %H:%M"))
            if end.props.arr_plat:
                print("       Platform:  %s" % end.props.arr_plat)
            print("       Line:      %s" % part.props.line)
            if end.props.rt_arrival:
                print("       Delay:     %s" % end.props.arrival_delay)
            if part.props.stops and len(part.props.stops) > 0:
                print("       %s Stops:   %s" % (len(part.props.stops),
                                                 ", ".join([s.props.name for s in part.props.stops])))
            else:
                print("       Stops:     0")
        print("")

def format_terse(trips):
    i = 0
    for trip in trips:
        i += 1
        msg = ' - CANCELED' if trip.props.status == Lpf.TripStatusFlags.CANCELED else ''
        print ('Trip #%d%s' % (i, msg))
        start = trip.props.parts[0].props.start
        end = trip.props.parts[-1].props.end
        print("       Start:     %s" % start.props.name)
        print("       Departure: %s" % start.props.departure.format("%F %H:%M"))
        print("       Delay:     %s" % start.props.departure_delay)
        print("       End:       %s" % end.props.name)
        print("       Arrival:   %s" % end.props.arrival.format("%F %H:%M"))
        print("       Delay:     %s" % end.props.arrival_delay)
        print("       Switches:  %d" % (len(trip.props.parts)-1))
        print("")


def trips_cb(trips, userdata, err):
    if err:
        quit(err.message)
        return

    if not trips:
        raise Exception("Failed to find any trips")
    if options.format == 'full':
        format_full(trips)
    else:
        format_terse(trips)
    quit()

def parse_datetime(when):
    if not when:
        return None

    date, time = when.split('T')
    y, m, d = date.split('-')
    H, M = time.split(':')
    return tuple(map(int, [y, m, d, H, M, '00']))

def main(argv):
    global mainloop, provider, options

    parser = optparse.OptionParser()
    parser.add_option("--provider", "-p", dest="provider",
                      help="Provider to use", default="de-db")
    parser.add_option("--format", dest="format",
                      help="Format to use (terse or full)", default="terse")
    parser.add_option("--when", dest="when",
                      help="When to start the trip", default=None)
    options, args = parser.parse_args(argv)

    if len(args) == 3:
        start = args[1]
        end = args[2]
    else:
        print("Usage: %s <from> <to>" % argv[0])
        return 1

    manager = Lpf.Manager()
    provider = manager.activate_provider(options.provider)
    print("Loaded provider %s" % provider.props.name)

    when = parse_datetime(options.when)
    provider.get_locs(start, 0, locs_cb, (end, when))

    mainloop = GObject.MainLoop()
    GObject.timeout_add_seconds(20, quit, "timed out")
    mainloop.run()
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))

