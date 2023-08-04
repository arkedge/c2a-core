{
  plugs: {
    mobc_ccsds: "ws://localhost:22545",
    mobc_uart: "ws://localhost:9696/channels/1",
    aobc_uart: "ws://localhost:9697/channels/0",
    gaia: "ws://localhost:8910",
  },
  links: {
    # uplink chain
    gaia: "mobc_ccsds",
    # mobc_ccsds -> |MOBC| -> mobc_uart
    mobc_uart: "aobc_uart",

    # downlink chain
    aobc_uart: "mobc_uart",
    # mobc_uart -> |MOBC| -> mobc_ccsds
    mobc_ccsds: "gaia",
  }
}
