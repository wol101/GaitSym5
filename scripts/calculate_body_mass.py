#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import xml.etree.ElementTree


def calculate_body_mass():
    parser = argparse.ArgumentParser(description="Parse an XML file and sum the Mass attributes of BODY tags.")
    parser.add_argument("xml_file", help="Path to the XML file to parse")
    args = parser.parse_args()

    tree = xml.etree.ElementTree.parse(args.xml_file)
    root = tree.getroot()

    # Find all BODY elements anywhere in the tree
    bodies = root.iter("BODY")

    total_mass = 0.0
    count = 0

    print(f"{'ID':<30} {'Mass':>15}")
    print("-" * 46)

    for body in bodies:
        body_id = body.get("ID", "<no ID>")
        mass_str = body.get("Mass")

        if mass_str is None:
            print(f"{body_id:<20} {'<no Mass>':>15}")
            continue

        mass = float(mass_str)
        total_mass += mass
        count += 1
        print(f"{body_id:<30} {mass:>15.6f}")

    print("-" * 46)
    print(f"{'Total (' + str(count) + ' bodies)':<30} {total_mass:>15.6f}")


if __name__ == "__main__":
    calculate_body_mass()
    