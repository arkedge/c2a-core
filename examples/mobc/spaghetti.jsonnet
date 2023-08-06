{
  plugs: {
    sils: "ws://localhost:22545",
    gaia: "ws://localhost:8910",
  },
  links: {
    # uplink chain
    gaia: "sils",

    # downlink chain
    sils: "gaia",
  }
}
