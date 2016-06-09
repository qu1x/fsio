-- This file is part of fsio, see <https://qu1x.org/fsio>.
-- 
-- Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>
-- 
-- fsio is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Affero General Public License version 3
-- as published by the Free Software Foundation on 19 November 2007.
-- 
-- fsio is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-- GNU Affero General Public License for more details.
-- 
-- You should have received a copy of the GNU Affero General Public License
-- along with fsio. If not, see <https://www.gnu.org/licenses>.

library ieee;
use ieee.std_logic_1164.all;

package fsio is
	constant CAP: integer := 32;
	constant LEN: integer := CAP;
	component fsio_get is
		generic (
			-- Number of signals of all data maps
			cap: integer := CAP;
			-- Number of signals of all data maps actually being used
			len: integer := LEN
		);
		port (
			-- AXI core clock
			clk: in std_logic;
			-- AXI handshake input
			hsi: in std_logic;
			-- AXI handshake output as conditional feedback
			hso: out std_logic;
			-- AXI data input of all data maps
			fsi: in std_logic_vector(cap - 1 downto 0);
			-- AXI data output as feedback of all data maps
			fso: out std_logic_vector(cap - 1 downto 0);
			-- User data of all data maps to be read
			dat: out std_logic_vector(len - 1 downto 0);
			-- PS requests PL to read the data
			-- Is set until acknowledged and the data has been fed back
			req: out std_logic;
			-- PL acknowledges when it has read the data
			-- Must be kept set until request has been reset
			ack: in std_logic
		);
	end component fsio_get;
	component fsio_put is
		generic (
			-- Number of signals of all data maps
			cap: integer := CAP;
			-- Number of signals of all data maps actually being used
			len: integer := LEN
		);
		port (
			-- AXI core clock
			clk: in std_logic;
			-- AXI handshake input
			hsi: in std_logic;
			-- AXI handshake output as conditional feedback
			hso: out std_logic;
			-- AXI data input as feedback of all data maps
			fsi: in std_logic_vector(cap - 1 downto 0);
			-- AXI data output of all data maps
			fso: out std_logic_vector(cap - 1 downto 0);
			-- User data of all data maps to be written
			dat: in std_logic_vector(len - 1 downto 0);
			-- PS requests PL to write the data
			-- Is set until acknowledged
			req: out std_logic;
			-- PL acknowledges when it has written the data
			-- Must be set for exactly one clock cycle
			ack: in std_logic
		);
	end component fsio_put;
end package;
