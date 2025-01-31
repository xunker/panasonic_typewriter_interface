# Panasonic Typewriter Interface

https://github.com/xunker/panasonic_typewriter_interface

# Model matrix and compatibility

Known models of Panasonic electronic typewriters and word processors, and
information on the computer interface they have (if any).

* `I/face?` - Does this model have an interface so it can be used as a printer?
  - If blank, it is unknown
* `Which` - What kind of interface?
  - `KX-R60` - MiniDIN-8 to Centronics adapter
  - `RP-K100` - DE-9 to RS-232C Serial or Centronics adapter
  - `RP-K105` - DE-9 to Centronics adapter
  - `KX-EIP2` - Add-in card that provides a Centronics port
  - `KX-EIP1` - Referenced in Panasonic manuals, but never seen elsewhere
  - `Centronics` - Typewriter has a built-in Centronics port

`Centronics` refers to a IEEE1284-compatible "Centronics" style parallel port.

Model    | I/face? | Which            | Notes
---------|---------|------------------|------
KX-R190  | no      |                  |
KX-R191  | yes     | KX-R60           | 15cps
KX-R192  |         |                  |
KX-R193  | yes     | KX-R60           | 12cps
KX-R194  | yes     | KX-R60           |
KX-R195  | yes     | KX-R60           |
KX-R200  | no      |                  | no port on side in pictures
KX-R210  |         |                  |
KX-R250  | no      |                  |
KX-R305  | no      |                  |
KX-R310  | no      |                  |
KX-R320  | no      |                  |
KX-R330  |         |                  |
KX-R340  |         |                  |
KX-R435  | yes     | KX-R60           | 12cps
KX-R445  |         |                  |
KX-R520  | no      |                  |
KX-R530  | yes     | KX-R60           |
KX-R540  |         |                  |
KX-R545  |         |                  |
KX-E500E |         |                  |
KX-E508E |         |                  |
KX-E601  |         |                  |
KX-E603  |         |                  |
KX-E700M |         |                  |
KX-E2000 | yes     | KX-EIP2          |
KX-E2020 | yes     | KX-EIP2          |
KX-E2500 | yes     | KX-EIP2          |
KX-E3000 |         |                  |
KX-E3500 | yes     | KX-EIP2          |
KX-E4020 |         |                  |
KX-E4500 |         |                  |
KX-E7000 | yes     | KX-EIP2          |
KX-W50TH | yes     | RP-K100          |
KX-W60TH | yes     | RP-K100          |
KX-W900  | no      |                  |
KX-W940  | no      |                  |
KX-W1000 | no      |                  |
KX-W1030 | no      |                  |
KX-W1525 |         |                  |
KX-WL50  | yes     | KX-R60, DB-25    | NiCD battery (1.2v * 10)
KX-WL55  | yes     | KX-R60, DB-25    |
RK-T25   | yes     | RP-K100          |
RK-T28   |         |                  |
RK-T30   |         |                  |
RK-T32   | yes     | RP-K100, RP-K105 |
RK-T33   |         |                  |
RK-T34   |         |                  |
RK-T35   |         |                  |
RK-T36   | yes     | RP-K100, RP-K105 |
RK-T37   |         |                  |
RK-T40   | yes     | Unknown          |
RK-T40D  |         |                  |
RK-T45   |         |                  |
RK-T55   |         | Centronics       | Ram card option, RP-K008
RK-H500  | yes     | Centronics       |

## Notes

> "It requires RP-K110/K111 serial cables which are the same except for some
> internal cabling I guess (null modem vs crossover cable?) What I'd like to
> have ..." -- https://mastodon.social/@paulrickards/109859819308983398

See also https://biosrhythm.com/?p=2143