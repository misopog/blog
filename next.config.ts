import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  output: "export",
  basePath: '/blog',
  assetPrefix:"/blog",
  typescript: {
    // !! WARN !!
    // Dangerously allow production builds to successfully complete even if
    // your project has type errors.
    // !! WARN !!
    ignoreBuildErrors: true,
  },
};

export default nextConfig;
