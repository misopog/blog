import { getAllPosts } from '@/lib/blog'
import { BlogPostCard } from '@/components/blog-post-card'
import Header from '@/components/header'

export default function Home() {
  const posts = getAllPosts()

  return (
    <div className="min-h-screen bg-black text-gray-100">
      <main className="max-w-5xl mx-auto py-12 px-4">
        <Header />
        <div className="mt-12 space-y-8">
          {posts.map((post) => (
            <BlogPostCard key={post.slug} post={post} />
          ))}
        </div>
      </main>
    </div>
  )
}

