/****************************************************************************
 * SVG fill									        					    *
 * 																			*
 * Copyright(C) 2020 AECgeeks and Bimforce								    *
 * 																		    *
 * This program is free software; you can redistribute it and/or		    *
 * modify it under the terms of the GNU Lesser General Public			    *
 * License as published by the Free Software Foundation; either			    *
 * version 3 of the License, or (at your option) any later version.		    *
 * 																		    *
 * This program is distributed in the hope that it will be useful,		    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of		    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	    *
 * Lesser General Public License for more details.						    *
 * 																		    *
 * You should have received a copy of the GNU Lesser General Public License *
 * along with this program; if not, write to the Free Software Foundation,  *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.	    *
 ****************************************************************************/

#include "svgfill.h"

#include <boost/optional.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <random>
#include <sstream>
#include <fstream>
#include <iostream>

namespace {
	std::string format_pt(const svgfill::point_2& p) {
		std::ostringstream oss;
		oss << p[0] << " " << p[1];
		return oss.str();
	}

	std::string format_poly(const svgfill::polygon_2& p) {
		std::ostringstream oss;
		for (auto it = p.begin(); it != p.end(); ++it) {
			oss << ((it == p.begin()) ? "M" : " L");
			oss << format_pt(*it);
		}
		oss << " Z";
		return oss.str();
	}
}

int main(int argc, char** argv) {
	bool valid_command_line = false;
	bool random_color = false;
	boost::optional<std::string> class_name;
	
	std::vector<std::string> flags;
	std::vector<std::string> args;

	for (int i = 1; i < argc; ++i) {
		std::string a = argv[i];
		if (boost::starts_with(a, "--")) {
			flags.push_back(a);
		}
		else {
			args.push_back(a);
		}
	}
	std::string fn, ofn;

	if (args.size() == 2) {
		fn = args[0];
		ofn = args[1];
		valid_command_line = true;
	}

	for (auto& f : flags) {
		if (f == "--random-color") {
			random_color = true;
		}
		else if (boost::starts_with(f, "--class=")) {
			class_name = f.substr(strlen("--class="));
		}
		else {
			valid_command_line = false;
		}
	}

	if (!valid_command_line) {
		std::cerr << "Usage: " << argv[0] << " [--random-color] [--class=...] <in.svg> <out.svg>" << std::endl;
 		return 1;
	}

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<size_t> dist(0, 360);

	std::vector<std::vector<svgfill::line_segment_2>> segments;
	std::vector<std::vector<svgfill::polygon_2>> polygons;

	if (svgfill::svg_to_line_segments(fn, class_name, segments) &&
		svgfill::line_segments_to_polygons(segments, polygons))
	{
		std::ofstream ofs(ofn.c_str());
		ofs << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">";
		ofs << "<style type=\"text/css\">";
		ofs << "	<![CDATA[";
		ofs << "		path {";
		ofs << "			stroke: #222222;";
		ofs << "			fill: #444444;";
		ofs << "		}";
		ofs << "	]]>";
		ofs << "</style>";

		for (auto& g : polygons) {
			ofs << "<g>";
			for (auto& p : g) {
				const int h = dist(mt);
				const int s = 50;
				const int l = 50;
				std::string style;
				if (random_color) {
					std::ostringstream oss;
					oss << "style = \"fill: hsl(" << h << "," << s << "%, " << l << "%)\"";
					style = oss.str();
				}
				ofs << "<path d=\"" << format_poly(p) << "\" " << style << "/>";
			}
			ofs << "</g>";
		}

		ofs << "</svg>";
	}
}
