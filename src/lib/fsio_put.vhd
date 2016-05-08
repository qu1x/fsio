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

library fsio;
use fsio.fsio.all;

entity fsio_put is
	generic (
		cap: integer := CAP;
		len: integer := LEN
	);
	port (
		clk: in std_logic;
		hsi: in std_logic;
		hso: out std_logic;
		fsi: in std_logic_vector(cap - 1 downto 0);
		fso: out std_logic_vector(cap - 1 downto 0);
		dat: in std_logic_vector(len - 1 downto 0);
		req: out std_logic;
		ack: in std_logic
	);
end fsio_put;

architecture behavioral of fsio_put is
begin
	fso(len - 1 downto 0) <= dat;
	req <= hso xor hsi;
	ctl: process(clk)
	begin
		if rising_edge(clk) then
			if fso = fsi then
				hso <= hso xor (req and ack);
			end if;
		end if;
	end process ctl;
end behavioral;
