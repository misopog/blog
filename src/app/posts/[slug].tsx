import fs from 'fs'
import path from 'path'
import matter from 'gray-matter'
import Link from 'next/link'
import { Markdown } from '../../components/markdown'

export async function generateStaticParams() {
  const postsDirectory = path.join(process.cwd(), 'src', 'posts')
  const fileNames = fs.readdirSync(postsDirectory)
  
  return fileNames.map((fileName) => ({
    slug: fileName.replace(/\.md$/, ''),
  }))
}

export default async function Post(props: { params: Promise<{ slug: string }> }) {
  const params = await props.params;
  const fullPath = path.join(process.cwd(), 'src', 'posts', `${params.slug}.md`)
  const fileContents = fs.readFileSync(fullPath, 'utf8')
  const { data, content } = matter(fileContents)

  return (
    <div className="min-h-screen bg-black text-gray-100">
      <main className="max-w-4xl mx-auto py-16 px-4">
        <Link href="/" className="text-blue-400 mb-8 block">&larr; Back to home</Link>
        <article>
          <h1 className="text-4xl font-bold mb-4">{data.title}</h1>
          <p className="text-gray-400 mb-8">{new Date(data.date).toLocaleDateString()}</p>
          <div className="prose prose-invert max-w-none">
            <Markdown>{content}</Markdown>
          </div>
        </article>
      </main>
    </div>
  )
}
