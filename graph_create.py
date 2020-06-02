from xml.etree import ElementTree as ET
import requests


class XMLParser(object):
    def __init__(self, file, isurl=False):
        self.nodes = []
        self.edges = []
        if isurl == True:
            self.xml = self.fromurl(file)
        else:
            self.xml = self.fromfile(file)
        self.parsexml()

    def fromurl(self, url):
        r = requests.get(url)
        if r.status == 200:
            return r.content
        else:
            raise Exception("File Not Found")

    def fromfile(self, file):
        stream = open(file, "r")
        xml = stream.read()
        stream.close()
        return xml

    def parsexml(self):
        root = ET.fromstring(self.xml)
        _nodes = root.findall("node")
        _ways = root.findall("way")
        idx = {}
        for i, node in enumerate(_nodes):
            node_id = int(node.get("id"))
            lat = float(node.get("lat"))
            lon = float(node.get("lon"))
            idx[node_id] = i + 1
            self.nodes.append((lat, lon))
        for way in _ways:
            paths = way.findall("nd")
            for i in range(len(paths) - 1):
                id1 = int(paths[i].get("ref"))
                id2 = int(paths[i + 1].get("ref"))
                idx1 = idx[id1]
                idx2 = idx[id2]
                self.edges.append((idx1, idx2))

    def writefiles(self):
        file1 = open("nodes.txt", "w")
        file2 = open("edges.txt", "w")
        lines1 = []
        lines2 = []
        lines1.append("{}\n".format(len(self.nodes)))
        lines2.append("{}\n".format(len(self.edges)))
        for node in self.nodes:
            lines1.append("{} {}\n".format(node[0], node[1]))
        for edge in self.edges:
            lines2.append("{} {}\n".format(edge[0], edge[1]))
        file1.writelines(lines1)
        file2.writelines(lines2)
        file1.close()
        file2.close()


if __name__ == "__main__":
    a = XMLParser("map")
    a.writefiles()
