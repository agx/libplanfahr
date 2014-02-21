#!/usr/bin/python
#
# Querry current trips between the two locations
# given on the command line

import sys

from gi.repository import Lpf
from gi.repository import GObject
from gi.repository import GLib

mainloop = None
start = None
end = None
provider = None

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
        print("Start: %s" % start.props.name)
        provider.get_locs(userdata, locs_cb, None)
    else:
        end = locs[0]
        print("End: %s" % end.props.name)
        now = GLib.DateTime.new_now_local()
        provider.get_trips(start, end, now, 0, trips_cb, None)


def trips_cb(trips, userdata, err):
    if err:
        quit(err.message)
        return

    if not trips:
        raise Exception("Failed to find any trips")
    i = 0
    for trip in trips:
        i += 1
        j = 0
        print ('Trip #%d' % i)
        for part in trip.props.parts:
            j += 1
            print('   Part #%d' % j)
            start = part.props.start
            end = part.props.end
            print("       Start:     %s" % start.props.name)
            print("       Departure: %s" % start.props.departure.format("%F %H:%M"))
            if start.props.dep_plat:
                print("       Platform:  %s" % start.props.dep_plat)
            print("       Delay:     %s" % start.props.departure_delay)
            print("       End:       %s" % end.props.name)
            print("       Arrival:   %s" % end.props.arrival.format("%F %H:%M"))
            if end.props.arr_plat:
                print("       Platform:  %s" % end.props.arr_plat)
            print("       Line:      %s" % part.props.line)
            print("       Delay:     %s" % end.props.arrival_delay)
            if part.props.stops and len(part.props.stops) > 0:
                print "       %s Stops:   %s" % (len(part.props.stops),
                                                 ", ".join([s.props.name for s in part.props.stops]))
            else:
                print "       Stops:     0"
        print("")
    quit()

def main(args):
    global mainloop, provider

    if len(args) == 3:
        start = args[1]
        end = args[2]
    else:
        print("Usage: %s <from> <to>" % args[0])
        return 1

    manager = Lpf.Manager()
    provider = manager.activate_provider("de-db")
    print("Loaded provider %s" % provider.props.name)

    provider.get_locs(start, locs_cb, end)

    mainloop = GObject.MainLoop()
    GObject.timeout_add_seconds(20, quit, "timed out")
    mainloop.run()
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv))

