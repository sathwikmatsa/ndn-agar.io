import argparse
import sys

from mininet.log import setLogLevel, info
from mininet.topo import Topo

from minindn.minindn import Minindn
from minindn.util import MiniNDNCLI
from minindn.apps.app_manager import AppManager
from minindn.apps.nfd import Nfd
from minindn.helpers.ndn_routing_helper import NdnRoutingHelper

from minindn.apps.tshark import Tshark

if __name__ == '__main__':
    setLogLevel('info')

    Minindn.cleanUp()
    Minindn.verifyDependencies()

    parser = argparse.ArgumentParser()
    parser.add_argument('--face-type', dest='faceType', default='udp', choices=['udp', 'tcp'])
    parser.add_argument('--routing', dest='routingType', default='link-state',
                         choices=['link-state', 'hr', 'dry'],
                         help='''Choose routing type, dry = link-state is used
                                 but hr is calculated for comparision.''')

    ndn = Minindn(parser=parser)

    ndn.start()

    ##########
    info('Starting tshark logging on nodes\n')
    tshark = AppManager(ndn, ndn.net.hosts, Tshark, logFolder="../log/", singleLogFile=True)
    #########
    
    info('Starting NFD on nodes\n')
    nfds = AppManager(ndn, ndn.net.hosts, Nfd)

    info('Adding static routes to NFD\n')
    grh = NdnRoutingHelper(ndn.net, ndn.args.faceType, ndn.args.routingType)

    info("\nConfiguring nodes\n\n")
    count_clients = 0
    for host in ndn.net.hosts:
        value = host.params['params'].get('type'," ")
        if (value=="server"):
            data_prefix = host.params['params'].get('prefix',"/ndnagario/server")
            # For all host, pass ndn.net.hosts or a list, [ndn.net['a'], ..] or [ndn.net.hosts[0],.]
            grh.addOrigin([ndn.net[str(host)]], [data_prefix])
            info(str(host) + " declared server for " + data_prefix+"\n")
            run_cmd = host.params['params'].get('run')
            if(run_cmd):
                run_cmd = run_cmd.replace("+"," ")
                command = run_cmd+ ' '+ str(count_clients) + ' &'
                host.cmd(command)
                info(str(host) + " is running "+ command +"\n")
        if(value=="client"):
            count_clients+=1
            data_prefix = host.params['params'].get('prefix',"/ndnagario/client")
            grh.addOrigin([ndn.net[str(host)]], [data_prefix])
            info(str(host) + " declared server for " + data_prefix+"\n")
            run_cmd = host.params['params'].get('run')
            if(run_cmd):
                run_cmd = run_cmd.replace("+"," ")
                host.cmd(run_cmd+' &')
                info(str(host) + " is running "+ run_cmd+ ' &'+ "\n")
    info("\n")
    grh.calculateNPossibleRoutes()
    info(str(count_clients) + ' clients to start the game. \n')
    info('Run <host> ../bin/agario_client to define consumers\n')
    info(str())
    info("Logs will be stored in /tmp/minindn/logs\n")

    MiniNDNCLI(ndn.net)

    ndn.stop()
