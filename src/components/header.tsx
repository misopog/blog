import Link from 'next/link'

export default function Header() {
  return (
    <header className="text-2xl font-bold mb-4">
      <nav className="flex justify-between items-center">
        <ul className="flex space-x-4">
          <li><Link href="/" className="hover:text-gray-400">blog</Link></li>
          <li><Link href="https://misopog.xyz" className="hover:text-gray-400">~</Link></li>
        </ul>
      </nav>
    </header>
  )
}

