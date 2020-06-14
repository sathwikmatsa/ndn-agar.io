echo "Copying assets to /tmp/minindn/assets"
sudo cp -r ../assets /tmp/minindn/
echo "Copying builds to /tmp/minindn/bin"
sudo cp -r ../bin /tmp/minindn/
echo "Running experiment"
sudo python experiment.py topology.conf;
# Merge pcaps
echo " "
echo "MiniNDN closed gracefully"
echo "Merging logs"
cd /tmp/minindn/log;
sudo rm merged.pcap 2> /dev/null
sudo mergecap -w merged.pcap `find *.pcap`
#sudo tshark -T text -Y udp -r merged.pcap
echo "Logs merged to /tmp/minindn/log/merged.pcap"
echo "Experiment completed"
