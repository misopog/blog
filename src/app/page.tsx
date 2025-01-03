import fs from 'fs'
import path from 'path'
import matter from 'gray-matter'
import Link from 'next/link'
import Header from '../components/header'

export default function Home() {
  const postsDirectory = path.join(process.cwd(), '/posts')
  const fileNames = fs.readdirSync(postsDirectory)
  
  const posts = fileNames.map((fileName) => {
    const fullPath = path.join(postsDirectory, fileName)
    const fileContents = fs.readFileSync(fullPath, 'utf8')
    const { data } = matter(fileContents)
    
    return {
      slug: fileName.replace(/\.md$/, ''),
      title: data.title,
      date: data.date,
    }
  })

  posts.sort((a, b) => new Date(b.date).getTime() - new Date(a.date).getTime())

  return (
    <div className="min-h-screen bg-black text-gray-100"> 
      <main className="max-w-4xl mx-auto py-8 px-4">
      <Header />
        <div className="space-y-4">
          {posts.map((post) => (
            <article key={post.slug} className="rounded-lg p-4 hover:bg-zinc-900 transition duration-300">
              <Link href={`/posts/${post.slug}`}>
                <h2 className="text-lg font-semibold mb-1">{post.title}</h2>
                <p className="text-gray-400">{new Date(post.date).toLocaleDateString()}</p>
              </Link>
            </article>
          ))}
        </div>
      </main>
    </div>
  )
}

