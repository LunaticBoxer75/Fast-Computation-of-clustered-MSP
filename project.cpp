#include<bits/stdc++.h>
using namespace std;
#define int long long
#define pi M_PI
#define R 6371e3

vector<vector<pair<int, int>>> v; // basic graph (input)

vector<vector<pair<int, int>>> v1; // reverse graph (input)

vector<pair<double, double>> m; // node to coordinates (input)

vector<int> p; // potential exit nodes

vector<pair<pair<int, int>, int>> edges; // all edges (input)

int n=0; // number of nodes

int ne=0; // number of edges

int ssp_comp=0; // To keep count of no of ssp computations 

int center;

vector<int> s; // sources

vector<int> ps; // potential sources

vector<int> r = {600, 800, 1000}; // set of radius to check

unsigned seed = rand(); // random seed

vector<int> t; // target nodes

map<int, int> ma; // map for differentiating

vector<int> ext; // exit nodes

vector<vector<pair<int, int>>> v2; // exit nodes to target nodes

map<int, map<int, int>> ans;

int pe_size=0; // No of potential exit nodes (Required for evaluation)

int base_steps=0; // baseline steps

int distance(int c1, int c2)
{
	double x1=m[c1].first;
	double y1=m[c1].second;
	double x2=m[c2].first;
	double y2=m[c2].second;
	double p1=(x1*pi)/180.0;
	double p2=(x2*pi)/180.0;
	double d1=((x2-x1)*pi)/180.0;
	double d2=((y2-y1)*pi)/180.0;
	double a=sin(d1/2.0)*sin(d1/2.0)+cos(p1)*cos(p2)*sin(d2/2.0)*sin(d2/2);
	double c=2*atan2(sqrt(a), sqrt(1-a));
	return (int)(c*R);
}

int distance2(double x1, double y1, double x2, double y2)
{
	double p1=(x1*pi)/180.0;
	double p2=(x2*pi)/180.0;
	double d1=((x2-x1)*pi)/180.0;
	double d2=((y2-y1)*pi)/180.0;
	double a=sin(d1/2.0)*sin(d1/2.0)+cos(p1)*cos(p2)*sin(d2/2.0)*sin(d2/2);
	double c=2*atan2(sqrt(a), sqrt(1-a));
	return (int)(c*R);
}

void input() // initializing the graph
{
	ifstream in1;
	in1.open("nodes.txt");
	in1>>n;
	m.resize(n+1);
	v.resize(n+1);
	v1.resize(n+1);
	v2.resize(n+1);
	for(int i=0;i<n;i++)
	{
		double x, y;
		in1>>x>>y;
		m[i+1]={x, y};
	}
	in1.close();
	ifstream in2;
	in2.open("edges.txt");
	in2>>ne;
	edges.resize(ne+1);
	for(int i=0;i<ne;i++)
	{
		int a, b;
		in2>>a>>b;
		int w=abs(distance(a, b));
		edges[i]={{a, b}, w};
		v[a].push_back({b, w});
		v[b].push_back({a, w});
		v1[a].push_back({b, w});
		v1[b].push_back({a, w});
	}
}

void reset(){
	p.clear();
	ps.clear();
	s.clear();
	t.clear();
	ma.clear();
	ext.clear();
	v2.clear();
	ans.clear();
	pe_size=0;
	ssp_comp=0;
	base_steps=0;
}

void select_center_and_source(int r, int num) // radius, number of source nodes
{
	while(true)
	{
		int node=(int)((rand()%n)+1);

		int x=m[node].first;
		int y=m[node].second;
		for(int i=1;i<=n;i++)
		{
			int xa=m[node].first;
			int ya=m[node].second;
			if(distance(node, i)<=r)
			{
				ps.push_back(i);
			}
		}
		if(ps.size()<num)
		{
			ps.clear();
			continue;
		}
		shuffle(ps.begin(), ps.end(), default_random_engine(seed));
		for(int i=0;i<num;i++)
		{
			s.push_back(ps[i]);
			ma[s[i]]=1;
		}
		center = node;
		break;
	}
}

void select_targets(int num)
{
	map<int, int> m1;
	vector<int> rest;
	for(int i=0;i<(int)ps.size();i++)
	{
		m1[ps[i]]=1;
	}
	for(int i=1;i<=n;i++)
	{
		if(!m1[i])
			rest.push_back(i);
	}
	shuffle(rest.begin(), rest.end(), default_random_engine(seed));
	for(int i=0;i<num;i++)
	{
		t.push_back(rest[i]);
		ma[rest[i]]=2;
	}
}

void potential_ext_nodes(int r)
{
	for(int i=0;i<ne;i++)
	{
		int d1=distance(edges[i].first.first, center);
		int d2=distance(edges[i].first.second, center);
		if(d1>d2)
		{
			swap(d1, d2);
			swap(edges[i].first.first, edges[i].first.second);
		}
		if(d1<r && d2>r)
		{
			p.push_back(edges[i].first.first);
			if(ma[edges[i].first.first]==1)	
				ma[edges[i].first.first]=13;
			else
				ma[edges[i].first.first]=3;
		}
	}
	pe_size=p.size();
}

void lpen_func()
{
	ssp_comp+=(int)p.size();
	for(int i=0;i<(int)p.size();i++)
	{
		int node=p[i];
		vector<int> dis(n+1, INT_MAX);
		dis[node]=0;
		vector<int> lpen(n+1, 0);
		vector<int> vis(n+1, 0);
		lpen[node]=node;
		int start=node;
		multiset<pair<int, int>> s;
		s.insert({0, node});
		int c=0;
		while(s.size()>0)
		{
			pair<int, int> p=*(s.begin());
			s.erase(s.begin());
			if(ma[p.second]==2)
				c++;
			if(c==(int)t.size())
				break;
			if(ma[p.second]==3 || ma[p.second]==13)
			{
				start=p.second;
			}
			lpen[p.second]=start;
			if(vis[p.second]==1)
				continue;
			vis[p.second]=1;
			for(int i=0;i<(int)(v[p.second].size());i++)
			{
				if(dis[p.second]+v[p.second][i].second<dis[v[p.second][i].first])
				{
					dis[v[p.second][i].first]=dis[p.second]+v[p.second][i].second;
					s.insert({dis[v[p.second][i].first], v[p.second][i].first});
				}
			}
		}
		int f=0;
		for(int i=0;i<(int)t.size();i++)
		{
			if(lpen[t[i]]==node)
			{
				if(f==0)
				{
					ext.push_back(node);
					f=1;
				}
				v2[node].push_back({t[i], dis[t[i]]});
			}
		}
	}
}

void reverse()
{
	ssp_comp+=(int)ext.size();
	for(int i=0;i<(int)ext.size();i++)
	{
		int node=ext[i];
		vector<int> dis(n+1, INT_MAX);
		vector<bool> vis(n+1, 0);
		int c=0;
		multiset<pair<int, int>> s1;
		dis[node]=0;
		s1.insert({0, node});
		while(s1.size()>0)
		{
			pair<int, int> p=*(s1.begin());
			s1.erase(s1.begin());
			if(vis[p.second])
				continue;
			vis[p.second]=1;
			if(ma[p.second]==1 || ma[p.second]==13)
				c++;
			if(c==(int)s.size())
			{
				break;
			}
			for(int i=0;i<(int)v[p.second].size();i++)
			{
				if(dis[p.second]+v[p.second][i].second<dis[v[p.second][i].first])
				{
					dis[v[p.second][i].first]=dis[p.second]+v[p.second][i].second;
					s1.insert({dis[v[p.second][i].first], v[p.second][i].first});
				}
			}
		}
		for(int i=0;i<(int)s.size();i++)
		{
			v2[s[i]].push_back({node, dis[s[i]]});
		}
	}
}

void final_step()
{
	for(int i=0;i<(int)s.size();i++)
	{
		vector<int> dis(n+1, INT_MAX);
		vector<int> vis(n+1, INT_MAX);
		dis[s[i]]=0;
		vector<int> vx;
		vx.push_back(s[i]);
		while(vx.size()>0)
		{
			int node=vx[(int)vx.size()-1];
			vx.pop_back();
			if(vis[node])
				continue;
			vis[node]=1;
			for(int i=0;i<(int)v2[node].size();i++)
			{
				dis[v2[node][i].second]=min(dis[v2[node][i].second], dis[node]+v2[node][i].first);
				if(ma[v2[node][i].first]==3 || ma[v2[node][i].first]==13)
				{
					if(!vis[v2[node][i].first])
						vx.push_back(v2[node][i].first);
				}
			}
		}
		for(int i=0;i<(int)t.size();i++)
		{
			if(dis[t[i]]!=INT_MAX)
			{
				ans[s[i]][t[i]]=dis[t[i]];
			}
		}
	}
}

void baseline()
{
	for(int i=0;i<(int)s.size();i++)
	{
		vector<int> dis(n+1, INT_MAX);
		vector<int> vis(n+1, 0);
		int node=s[i];
		dis[node]=0;
		multiset<pair<int, int>> s1;
		s1.insert({0, node});
		int c=0;
		while(s1.size()>0)
		{
			pair<int, int> p=*(s1.begin());
			s1.erase(s1.begin());
			if(vis[p.second])
				continue;
			if(ma[p.second]==2)
				c++;
			if(c==(int)t.size())
				break;
			vis[p.second]=1;
			for(int i=0;i<(int)v[p.second].size();i++)
			{
				if(dis[p.second]+v[p.second][i].second<dis[v[p.second][i].first])
				{
					dis[v[p.second][i].first]=dis[p.second]+v[p.second][i].second;
					s1.insert({dis[v[p.second][i].first], v[p.second][i].first});
				}
			}
		}
	}
	base_steps=(int)s.size();
}

void evaluate_and_run(){
	
	ofstream fout,fout1;
	fout.open( "evaluation_data1.txt");
	fout1.open( "potential_exit_nodes1.txt" );

	for(int x=0;x<(int)r.size();x++)
	{
		fout<<"radius : "<<r[x]<<endl;
		fout1<<"radius : "<<r[x]<<endl;

		for(int y=1;y<=5;y+=1)
		{	
			int num=(r[x]*y);
			double avg_runspeedup=0,avg_ssp_speedup=0;
			for(int itr=1;itr<=50;itr++)
			{
				reset();
				input();

				clock_t start,end;
				double time1,time2;
				start=clock();

				select_center_and_source(r[x],num);
				select_targets(num);
				potential_ext_nodes(r[x]);
				lpen_func();
				reverse();
				final_step();

				end=clock();
				time1=double(end-start)/double(CLOCKS_PER_SEC);

				start=clock();

				baseline();

				end=clock();
				time2=double(end-start)/double(CLOCKS_PER_SEC);
				avg_runspeedup+=time2/time1;
				avg_ssp_speedup+=(base_steps*1.0)/ssp_comp;
				fout1<<pe_size<<" ";
			}
			avg_runspeedup/=50;
			avg_ssp_speedup/=50;
			fout<<"|S|/R: "<<y<<" avg_runspeedup: "<<avg_runspeedup<<" avg_ssp_speedup "<<avg_ssp_speedup<<endl;
		}
		fout<<endl;
		fout1<<endl;
	}
	fout.close();
	fout1.close();
}

int32_t main()
{
	evaluate_and_run();
}
