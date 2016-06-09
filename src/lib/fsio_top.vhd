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
use ieee.numeric_std.all;

library fsio;
use fsio.fsio.all;

entity fsio_top is
	port (
		DDR_addr : inout std_logic_vector(14 downto 0);
		DDR_ba : inout std_logic_vector(2 downto 0);
		DDR_cas_n : inout std_logic;
		DDR_ck_n : inout std_logic;
		DDR_ck_p : inout std_logic;
		DDR_cke : inout std_logic;
		DDR_cs_n : inout std_logic;
		DDR_dm : inout std_logic_vector(3 downto 0);
		DDR_dq : inout std_logic_vector(31 downto 0);
		DDR_dqs_n : inout std_logic_vector(3 downto 0);
		DDR_dqs_p : inout std_logic_vector(3 downto 0);
		DDR_odt : inout std_logic;
		DDR_ras_n : inout std_logic;
		DDR_reset_n : inout std_logic;
		DDR_we_n : inout std_logic;
		FIXED_IO_ddr_vrn : inout std_logic;
		FIXED_IO_ddr_vrp : inout std_logic;
		FIXED_IO_mio : inout std_logic_vector(53 downto 0);
		FIXED_IO_ps_clk : inout std_logic;
		FIXED_IO_ps_porb : inout std_logic;
		FIXED_IO_ps_srstb : inout std_logic
	);
end fsio_top;

architecture behavioral of fsio_top is
	component ps is
		port (
			DDR_cas_n : inout std_logic;
			DDR_cke : inout std_logic;
			DDR_ck_n : inout std_logic;
			DDR_ck_p : inout std_logic;
			DDR_cs_n : inout std_logic;
			DDR_reset_n : inout std_logic;
			DDR_odt : inout std_logic;
			DDR_ras_n : inout std_logic;
			DDR_we_n : inout std_logic;
			DDR_ba : inout std_logic_vector(2 downto 0);
			DDR_addr : inout std_logic_vector(14 downto 0);
			DDR_dm : inout std_logic_vector(3 downto 0);
			DDR_dq : inout std_logic_vector(31 downto 0);
			DDR_dqs_n : inout std_logic_vector(3 downto 0);
			DDR_dqs_p : inout std_logic_vector(3 downto 0);
			FIXED_IO_mio : inout std_logic_vector(53 downto 0);
			FIXED_IO_ddr_vrn : inout std_logic;
			FIXED_IO_ddr_vrp : inout std_logic;
			FIXED_IO_ps_srstb : inout std_logic;
			FIXED_IO_ps_clk : inout std_logic;
			FIXED_IO_ps_porb : inout std_logic;

			clk180 : out std_logic;

			hsi0 : in std_logic_vector(0 to 0);
			hso0 : out std_logic_vector(0 to 0);

			fsi1 : in std_logic_vector(31 downto 0);
			fso1 : out std_logic_vector(31 downto 0);
			fsi2 : in std_logic_vector(31 downto 0);
			fso2 : out std_logic_vector(31 downto 0);
			fsi3 : in std_logic_vector(31 downto 0);
			fso3 : out std_logic_vector(31 downto 0);
			fsi4 : in std_logic_vector(31 downto 0);
			fso4 : out std_logic_vector(31 downto 0);

			hsi5 : in std_logic_vector(0 to 0);
			hso5 : out std_logic_vector(0 to 0);

			fsi6 : in std_logic_vector(31 downto 0);
			fso6 : out std_logic_vector(31 downto 0);
			fsi7 : in std_logic_vector(31 downto 0);
			fso7 : out std_logic_vector(31 downto 0);
			fsi8 : in std_logic_vector(31 downto 0);
			fso8 : out std_logic_vector(31 downto 0);
			fsi9 : in std_logic_vector(31 downto 0);
			fso9 : out std_logic_vector(31 downto 0)
		);
	end component ps;

	signal clk180: std_logic;

	signal req0, ack0: std_logic;
	signal hsi0, hso0: std_logic;
	signal fsi0, fso0, dat0: std_logic_vector(127 downto 0);

	signal req1, ack1: std_logic;
	signal hsi1, hso1: std_logic;
	signal fsi1, fso1, dat1: std_logic_vector(127 downto 0);

	signal data: std_logic_vector(127 downto 0);
	signal full: std_logic;
begin
	ps0: component ps
	port map (
		DDR_addr(14 downto 0) => DDR_addr(14 downto 0),
		DDR_ba(2 downto 0) => DDR_ba(2 downto 0),
		DDR_cas_n => DDR_cas_n,
		DDR_ck_n => DDR_ck_n,
		DDR_ck_p => DDR_ck_p,
		DDR_cke => DDR_cke,
		DDR_cs_n => DDR_cs_n,
		DDR_dm(3 downto 0) => DDR_dm(3 downto 0),
		DDR_dq(31 downto 0) => DDR_dq(31 downto 0),
		DDR_dqs_n(3 downto 0) => DDR_dqs_n(3 downto 0),
		DDR_dqs_p(3 downto 0) => DDR_dqs_p(3 downto 0),
		DDR_odt => DDR_odt,
		DDR_ras_n => DDR_ras_n,
		DDR_reset_n => DDR_reset_n,
		DDR_we_n => DDR_we_n,
		FIXED_IO_ddr_vrn => FIXED_IO_ddr_vrn,
		FIXED_IO_ddr_vrp => FIXED_IO_ddr_vrp,
		FIXED_IO_mio(53 downto 0) => FIXED_IO_mio(53 downto 0),
		FIXED_IO_ps_clk => FIXED_IO_ps_clk,
		FIXED_IO_ps_porb => FIXED_IO_ps_porb,
		FIXED_IO_ps_srstb => FIXED_IO_ps_srstb,

		clk180 => clk180,

		hsi0(0) => hso0,
		hso0(0) => hsi0,

		fsi1(31 downto 0) => fso0(31 downto 0),
		fso1(31 downto 0) => fsi0(31 downto 0),
		fsi2(31 downto 0) => fso0(63 downto 32),
		fso2(31 downto 0) => fsi0(63 downto 32),
		fsi3(31 downto 0) => fso0(95 downto 64),
		fso3(31 downto 0) => fsi0(95 downto 64),
		fsi4(31 downto 0) => fso0(127 downto 96),
		fso4(31 downto 0) => fsi0(127 downto 96),

		hsi5(0) => hso1,
		hso5(0) => hsi1,

		fsi6(31 downto 0) => fso1(31 downto 0),
		fso6(31 downto 0) => fsi1(31 downto 0),
		fsi7(31 downto 0) => fso1(63 downto 32),
		fso7(31 downto 0) => fsi1(63 downto 32),
		fsi8(31 downto 0) => fso1(95 downto 64),
		fso8(31 downto 0) => fsi1(95 downto 64),
		fsi9(31 downto 0) => fso1(127 downto 96),
		fso9(31 downto 0) => fsi1(127 downto 96)
	);

	fsio0: fsio_put
	generic map (
		cap => 128,
		len => 128
	)
	port map (
		clk => clk180,
		hsi => hsi0,
		hso => hso0,
		fsi => fsi0,
		fso => fso0,
		dat => dat0,
		req => req0,
		ack => ack0
	);

	fsio1: fsio_get
	generic map (
		cap => 128,
		len => 128
	)
	port map (
		clk => clk180,
		hsi => hsi1,
		hso => hso1,
		fsi => fsi1,
		fso => fso1,
		dat => dat1,
		req => req1,
		ack => ack1
	);

	ctl: process(clk180)
	begin
		if rising_edge(clk180) then
			if req0 then
				if full and not ack0 then
					dat0 <= data;
					full <= '0';
					ack0 <= '1';
				end if;
			else
				ack0 <= '0';
			end if;

			ack1 <= '0';
			if req1 then
				if not full then
					data <= dat1;
					full <= '1';
					ack1 <= '1';
				end if;
			end if;
		end if;
	end process ctl;
end behavioral;
